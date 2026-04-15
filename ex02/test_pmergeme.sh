#!/usr/bin/env bash
# =============================================================================
# test_pmergeme.sh — Test harness for PmergeMe (cpp09 ex02)
# =============================================================================

# ------------- Configuration -------------------------------------------------
BINARY="./PmergeMe"
MAX_JOBS=8
RUNS_PER_SIZE=5
LOG_FILE="pmergeme_failures.log"

RAND_SIZES=(
    2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
    25 32 33 50 64 65 100 128 129 256 257 500 512 513
    1000 2000 3000
)

# ------------- Temp workspace ------------------------------------------------
TMPDIR_ROOT="$(mktemp -d /tmp/pmergeme_test.XXXXXX)"
trap 'rm -rf "$TMPDIR_ROOT"' EXIT

# ------------- Helpers -------------------------------------------------------

parse_time_us() {
    local time_part
    time_part="$(echo "$1" | sed 's/.*: //')"
    local s ms us
    s=$(echo  "$time_part" | awk '{print $1}')
    ms=$(echo "$time_part" | awk '{print $3}')
    us=$(echo "$time_part" | awk '{print $5}')
    echo $(( s * 1000000 + ms * 1000 + us ))
}

avg_float() {
    (( $2 == 0 )) && { echo "0.0"; return; }
    awk "BEGIN { printf \"%.1f\", $1 / $2 }"
}

# us_to_smsu MICROSECONDS — prints "s/ms/us" e.g. "0/1/532.4"
# Accepts integers or floats (via awk)
us_to_smsu() {
    awk "BEGIN {
        total = $1
        s  = int(total / 1000000)
        ms = int((total % 1000000) / 1000)
        us = total - s * 1000000 - ms * 1000
        printf \"%d/%d/%.1f\", s, ms, us
    }"
}

# Repeat a character N times (loop handles multi-byte Unicode chars correctly)
repeat_char() {
    local char="$1" count="$2" i
    for (( i = 0; i < count; i++ )); do printf '%s' "$char"; done
}

# ------------- Core test runner ----------------------------------------------
# run_test  out_file  type  label  num [num ...]
run_test() {
    local out_file="$1"
    local type="$2"
    local label="$3"
    shift 3
    local input=("$@")

    local raw
    raw="$("$BINARY" "${input[@]}" 2>&1)"

    local sorted_line cmp_line tvec_line tdeq_line
    sorted_line=$(echo "$raw" | grep '^sorted:')
    cmp_line=$(   echo "$raw" | grep '^comparisons:')
    tvec_line=$(  echo "$raw" | grep 'std::vector')
    tdeq_line=$(  echo "$raw" | grep 'std::deque')

    local vec_sorted deq_sorted
    vec_sorted=$(echo "$sorted_line" | grep -o 'vector = [a-z]*' | awk '{print $3}')
    deq_sorted=$(echo "$sorted_line" | grep -o 'deque = [a-z]*'  | awk '{print $3}')

    local cmp_max cmp_vec cmp_deq
    cmp_max=$(echo "$cmp_line" | grep -o 'max = [0-9]*'    | awk '{print $3}')
    cmp_vec=$(echo "$cmp_line" | grep -o 'vector = [0-9]*' | awk '{print $3}')
    cmp_deq=$(echo "$cmp_line" | grep -o 'deque = [0-9]*'  | awk '{print $3}')

    local t_vec_us=0 t_deq_us=0
    [[ -n "$tvec_line" ]] && t_vec_us=$(parse_time_us "$tvec_line")
    [[ -n "$tdeq_line" ]] && t_deq_us=$(parse_time_us "$tdeq_line")

    local fail_reasons=""
    if [[ -z "$sorted_line" || -z "$cmp_line" || -z "$tvec_line" || -z "$tdeq_line" ]]; then
        fail_reasons="missing_output_lines"
    fi
    [[ "$vec_sorted" != "true" ]] && \
        fail_reasons="${fail_reasons:+$fail_reasons, }vector_not_sorted"
    [[ "$deq_sorted" != "true" ]] && \
        fail_reasons="${fail_reasons:+$fail_reasons, }deque_not_sorted"
    if [[ "$cmp_vec" =~ ^[0-9]+$ && "$cmp_max" =~ ^[0-9]+$ ]] && (( cmp_vec > cmp_max )); then
        fail_reasons="${fail_reasons:+$fail_reasons, }vec_cmp_exceeded(${cmp_vec}>${cmp_max})"
    fi
    if [[ "$cmp_deq" =~ ^[0-9]+$ && "$cmp_max" =~ ^[0-9]+$ ]] && (( cmp_deq > cmp_max )); then
        fail_reasons="${fail_reasons:+$fail_reasons, }deq_cmp_exceeded(${cmp_deq}>${cmp_max})"
    fi

    local status="PASS"
    [[ -n "$fail_reasons" ]] && status="FAIL"

    printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
        "$type" "$label" "$status" \
        "${vec_sorted:-?}" "${deq_sorted:-?}" \
        "${cmp_max:-?}" "${cmp_vec:-?}" "${cmp_deq:-?}" \
        "$t_vec_us" "$t_deq_us" \
        "${fail_reasons}" \
        > "$out_file"

    if [[ "$status" == "FAIL" ]]; then
        {
            echo "========================================"
            echo "LABEL  : $label"
            echo "INPUT  : ${input[*]}"
            echo "REASONS: $fail_reasons"
            echo "--- relevant lines ---"
            echo "$sorted_line"
            echo "$cmp_line"
            echo "$tvec_line"
            echo "$tdeq_line"
            echo ""
        } >> "$LOG_FILE"
    fi
}

# ------------- Job pool ------------------------------------------------------
declare -a _PIDS=()
_JOB_COUNTER=0

throttle_jobs() {
    while (( ${#_PIDS[@]} >= MAX_JOBS )); do
        local new_pids=() i
        for i in "${!_PIDS[@]}"; do
            kill -0 "${_PIDS[$i]}" 2>/dev/null && new_pids+=("${_PIDS[$i]}")
        done
        _PIDS=("${new_pids[@]:-}")
        (( ${#_PIDS[@]} >= MAX_JOBS )) && sleep 0.05
    done
}

# launch_test  type  label  num [num ...]
launch_test() {
    local type="$1"
    local label="$2"
    shift 2
    local nums=("$@")

    _JOB_COUNTER=$(( _JOB_COUNTER + 1 ))
    local out_file="$TMPDIR_ROOT/job_${_JOB_COUNTER}.result"

    throttle_jobs
    run_test "$out_file" "$type" "$label" "${nums[@]}" &
    _PIDS+=($!)
}

# ------------- Hardcoded edge cases ------------------------------------------
run_edge_cases() {
    launch_test edge "already sorted (2)"   1 2
    launch_test edge "reverse sorted (2)"   2 1
    launch_test edge "equal pair"           7 7
    launch_test edge "already sorted (5)"   1 2 3 4 5
    launch_test edge "reverse sorted (5)"   5 4 3 2 1
    launch_test edge "all equal (5)"        3 3 3 3 3
    launch_test edge "single swap needed"   1 3 2
    launch_test edge "size 3"               3 1 2
    launch_test edge "size 4"               4 3 2 1
    launch_test edge "size 5"               5 1 4 2 3
    launch_test edge "size 8"               8 7 6 5 4 3 2 1
    launch_test edge "size 9"               9 1 8 2 7 3 6 4 5
    launch_test edge "size 16"              16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
    launch_test edge "size 17"              17 1 16 2 15 3 14 4 13 5 12 6 11 7 10 8 9
    launch_test edge "sorted 3000"          $(seq 1 3000)
    launch_test edge "reversed 3000"        $(seq 3000 -1 1)
}

# ------------- Randomized tests ----------------------------------------------
run_random_tests() {
    local size run nums_str
    for size in "${RAND_SIZES[@]}"; do
        for (( run = 1; run <= RUNS_PER_SIZE; run++ )); do
            nums_str=$(shuf -i 1-1000000 -n "$size" | tr '\n' ' ')
            # shellcheck disable=SC2086
            launch_test rand "size=${size} run=${run}" $nums_str
        done
    done
}

# ------------- Display -------------------------------------------------------

# hline WIDTH CHAR — prints a horizontal line of CHAR repeated WIDTH times
hline() { repeat_char "$2" "$1"; echo; }

# boxline INNER_WIDTH TEXT — prints │ TEXT padded to INNER_WIDTH │
# (TEXT is already formatted to exactly INNER_WIDTH chars by the caller)
pad_line() { printf "│%s│\n" "$1"; }

print_results() {
    local edge_files=() rand_files=()
    local f type

    for f in "$TMPDIR_ROOT"/job_*.result; do
        [[ -f "$f" ]] || continue
        type=$(cut -f1 "$f")
        if [[ "$type" == "edge" ]]; then
            edge_files+=("$f")
        else
            rand_files+=("$f")
        fi
    done

    echo ""
    echo "╔══════════════════════════════════════════╗"
    echo "║         PmergeMe  Test  Results          ║"
    echo "╚══════════════════════════════════════════╝"

    # =========================================================================
    # EDGE CASES
    # Columns: Label | OK? | cmpV/max | cmpD/max | T_vec(us) | T_deq(us)
    # =========================================================================

    # First pass — measure widest value per column
    local w_label=5   # min = "Label"
    local w_cmpv=8    # min = "cmpV/max"
    local w_cmpd=8    # min = "cmpD/max"
    local w_tvec=14   # min = "T_vec(s/ms/us)"
    local w_tdeq=14   # min = "T_deq(s/ms/us)"
    local w_ok=4      # fixed: "PASS"/"FAIL"

    for f in "${edge_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r _t label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local cv_d="${cmp_vec}/${cmp_max}"
        local cd_d="${cmp_deq}/${cmp_max}"
        local tv_fmt td_fmt
        tv_fmt=$(us_to_smsu "$t_vec")
        td_fmt=$(us_to_smsu "$t_deq")
        (( ${#label}  > w_label )) && w_label=${#label}
        (( ${#cv_d}   > w_cmpv  )) && w_cmpv=${#cv_d}
        (( ${#cd_d}   > w_cmpd  )) && w_cmpd=${#cd_d}
        (( ${#tv_fmt} > w_tvec  )) && w_tvec=${#tv_fmt}
        (( ${#td_fmt} > w_tdeq  )) && w_tdeq=${#td_fmt}
    done

    # e_inner computed after all width checks — including header strings
    local e_inner=$(( w_label + w_ok + w_cmpv + w_cmpd + w_tvec + w_tdeq + 5*3 + 2 ))

    echo ""
    printf "┌─ HARDCODED EDGE CASES "; repeat_char "─" $(( e_inner - 23 )); printf "┐\n"
    printf "│ %-${w_label}s │ %-${w_ok}s │ %${w_cmpv}s │ %${w_cmpd}s │ %${w_tvec}s │ %${w_tdeq}s │\n" \
        "Label" "OK?" "cmpV/max" "cmpD/max" "T_vec(s/ms/us)" "T_deq(s/ms/us)"
    printf "│"; repeat_char "─" $(( e_inner )); printf "│\n"

    local edge_pass=0 edge_fail=0
    for f in "${edge_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r _t label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local cv_d="${cmp_vec}/${cmp_max}"
        local cd_d="${cmp_deq}/${cmp_max}"
        local tv_fmt td_fmt
        tv_fmt=$(us_to_smsu "$t_vec")
        td_fmt=$(us_to_smsu "$t_deq")
        printf "│ %-${w_label}s │ %-${w_ok}s │ %${w_cmpv}s │ %${w_cmpd}s │ %${w_tvec}s │ %${w_tdeq}s │\n" \
            "$label" "$status" "$cv_d" "$cd_d" "$tv_fmt" "$td_fmt"
        if [[ "$status" == "FAIL" ]]; then
            printf "│  ↳ %-$(( e_inner - 4 ))s │\n" "$reasons"
            edge_fail=$(( edge_fail + 1 ))
        else
            edge_pass=$(( edge_pass + 1 ))
        fi
    done
    printf "└"; repeat_char "─" $(( e_inner )); printf "┘\n"
    printf "  Edge cases : %d passed, %d failed\n" "$edge_pass" "$edge_fail"

    # =========================================================================
    # RANDOMIZED TESTS
    # Columns: Size | Runs | Vec✓ | Deq✓ | AvgCmpV | AvgCmpD | AvgT_vec | AvgT_deq | Faster
    # =========================================================================

    # Aggregate per size first so we can measure column widths before printing
    declare -A sz_runs sz_vpass sz_dpass sz_fail
    declare -A sz_scv sz_scd sz_stv sz_std

    local rand_pass=0 rand_fail=0

    for f in "${rand_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r _t label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local sz
        sz=$(echo "$label" | grep -o 'size=[0-9]*' | cut -d= -f2)
        [[ -z "$sz" ]] && continue

        sz_runs[$sz]=$(( ${sz_runs[$sz]:-0} + 1 ))
        [[ "$vec_s" == "true" ]] && sz_vpass[$sz]=$(( ${sz_vpass[$sz]:-0} + 1 ))
        [[ "$deq_s" == "true" ]] && sz_dpass[$sz]=$(( ${sz_dpass[$sz]:-0} + 1 ))
        [[ "$status" == "FAIL" ]] && sz_fail[$sz]=$(( ${sz_fail[$sz]:-0} + 1 ))
        [[ "$cmp_vec" =~ ^[0-9]+$ ]] && sz_scv[$sz]=$(( ${sz_scv[$sz]:-0} + cmp_vec ))
        [[ "$cmp_deq" =~ ^[0-9]+$ ]] && sz_scd[$sz]=$(( ${sz_scd[$sz]:-0} + cmp_deq ))
        [[ "$t_vec"   =~ ^[0-9]+$ ]] && sz_stv[$sz]=$(( ${sz_stv[$sz]:-0} + t_vec ))
        [[ "$t_deq"   =~ ^[0-9]+$ ]] && sz_std[$sz]=$(( ${sz_std[$sz]:-0} + t_deq ))

        if [[ "$status" == "PASS" ]]; then
            rand_pass=$(( rand_pass + 1 ))
        else
            rand_fail=$(( rand_fail + 1 ))
        fi
    done

    # Build display rows into an array so we can measure widths before printing
    local -a rand_rows=()
    local total_tv=0 total_td=0 total_rand=0

    local w_sz=4 w_runs=4 w_vok=4 w_dok=4 w_acv=7 w_acd=7 w_atv=14 w_atd=14 w_faster=6

    for sz in "${RAND_SIZES[@]}"; do
        local runs=${sz_runs[$sz]:-0}
        (( runs == 0 )) && continue
        local vp=${sz_vpass[$sz]:-0}
        local dp=${sz_dpass[$sz]:-0}
        local fc=${sz_fail[$sz]:-0}
        local acv acd atv atd
        acv=$(avg_float "${sz_scv[$sz]:-0}" "$runs")
        acd=$(avg_float "${sz_scd[$sz]:-0}" "$runs")
        atv=$(us_to_smsu "$(avg_float "${sz_stv[$sz]:-0}" "$runs")")
        atd=$(us_to_smsu "$(avg_float "${sz_std[$sz]:-0}" "$runs")")

        local vok="${vp}/${runs}"
        local dok="${dp}/${runs}"
        local faster="tie"
        (( ${sz_stv[$sz]:-0} < ${sz_std[$sz]:-0} )) && faster="vector"
        (( ${sz_std[$sz]:-0} < ${sz_stv[$sz]:-0} )) && faster="deque"

        local flag=""
        (( fc > 0 )) && flag=" !"

        # store as tab-separated row: sz runs vok dok acv acd atv atd faster flag
        rand_rows+=("${sz}"$'\t'"${runs}"$'\t'"${vok}"$'\t'"${dok}"$'\t'"${acv}"$'\t'"${acd}"$'\t'"${atv}"$'\t'"${atd}"$'\t'"${faster}"$'\t'"${flag}")

        # track max widths
        (( ${#sz}    > w_sz    )) && w_sz=${#sz}
        (( ${#runs}  > w_runs  )) && w_runs=${#runs}
        (( ${#vok}   > w_vok   )) && w_vok=${#vok}
        (( ${#dok}   > w_dok   )) && w_dok=${#dok}
        (( ${#acv}   > w_acv   )) && w_acv=${#acv}
        (( ${#acd}   > w_acd   )) && w_acd=${#acd}
        (( ${#atv}   > w_atv   )) && w_atv=${#atv}
        (( ${#atd}   > w_atd   )) && w_atd=${#atd}
        (( ${#faster}> w_faster)) && w_faster=${#faster}

        total_tv=$(( total_tv + ${sz_stv[$sz]:-0} ))
        total_td=$(( total_td + ${sz_std[$sz]:-0} ))
        total_rand=$(( total_rand + runs ))
    done

    # header labels
    local hd_sz="Size" hd_runs="Runs" hd_vok="Vec✓" hd_dok="Deq✓"
    local hd_acv="AvgCmpV" hd_acd="AvgCmpD" hd_atv="AvgT_vec(s/ms/us)" hd_atd="AvgT_deq(s/ms/us)" hd_f="Faster"
    (( ${#hd_sz}  > w_sz  )) && w_sz=${#hd_sz}
    (( ${#hd_runs}> w_runs)) && w_runs=${#hd_runs}
    (( ${#hd_vok} > w_vok )) && w_vok=${#hd_vok}
    (( ${#hd_dok} > w_dok )) && w_dok=${#hd_dok}
    (( ${#hd_acv} > w_acv )) && w_acv=${#hd_acv}
    (( ${#hd_acd} > w_acd )) && w_acd=${#hd_acd}
    (( ${#hd_atv} > w_atv )) && w_atv=${#hd_atv}
    (( ${#hd_atd} > w_atd )) && w_atd=${#hd_atd}
    (( ${#hd_f}   > w_faster)) && w_faster=${#hd_f}

    local r_inner=$(( w_sz + w_runs + w_vok + w_dok + w_acv + w_acd + w_atv + w_atd + w_faster + 8*3 + 2 ))

    echo ""
    printf "┌─ RANDOMIZED TESTS "; repeat_char "─" $(( r_inner - 19 )); printf "┐\n"
    printf "│ %${w_sz}s │ %${w_runs}s │ %${w_vok}s │ %${w_dok}s │ %${w_acv}s │ %${w_acd}s │ %${w_atv}s │ %${w_atd}s │ %-${w_faster}s │\n" \
        "$hd_sz" "$hd_runs" "$hd_vok" "$hd_dok" "$hd_acv" "$hd_acd" "$hd_atv" "$hd_atd" "$hd_f"
    printf "│"; repeat_char "─" $(( r_inner )); printf "│\n"

    for row in "${rand_rows[@]}"; do
        IFS=$'\t' read -r sz runs vok dok acv acd atv atd faster flag <<< "$row"
        printf "│ %${w_sz}s │ %${w_runs}s │ %${w_vok}s │ %${w_dok}s │ %${w_acv}s │ %${w_acd}s │ %${w_atv}s │ %${w_atd}s │ %-${w_faster}s │%s\n" \
            "$sz" "$runs" "$vok" "$dok" "$acv" "$acd" "$atv" "$atd" "$faster" "$flag"
    done
    printf "└"; repeat_char "─" $(( r_inner )); printf "┘\n"
    printf "  Randomized : %d passed, %d failed\n" "$rand_pass" "$rand_fail"

    # =========================================================================
    # TIMING SUMMARY
    # =========================================================================
    local avg_tv_raw avg_td_raw avg_tv avg_td
    avg_tv_raw=$(avg_float "$total_tv" "$total_rand")
    avg_td_raw=$(avg_float "$total_td" "$total_rand")
    avg_tv=$(us_to_smsu "$avg_tv_raw")
    avg_td=$(us_to_smsu "$avg_td_raw")

    local faster_overall="tied"
    (( total_tv < total_td )) && faster_overall="vector"
    (( total_td < total_tv )) && faster_overall="deque"

    # Build timing rows as unpadded strings, find the longest, then pad all to match
    local tlabel1="  Avg T_vec (s/ms/us) (randomized) : "
    local tlabel2="  Avg T_deq (s/ms/us) (randomized) : "
    local tlabel3="  Overall faster                    : "
    local tval1="$avg_tv"
    local tval2="$avg_td"
    local tval3="$faster_overall"

    # inner width = longest label+value + 2 spaces padding on the right
    local t_inner=$(( ${#tlabel1} + ${#tval1} ))
    (( ${#tlabel2} + ${#tval2} > t_inner )) && t_inner=$(( ${#tlabel2} + ${#tval2} ))
    (( ${#tlabel3} + ${#tval3} > t_inner )) && t_inner=$(( ${#tlabel3} + ${#tval3} ))
    t_inner=$(( t_inner + 2 ))  # trailing padding

    echo ""
    printf "┌─ TIMING SUMMARY "; repeat_char "─" $(( t_inner - 17 )); printf "┐\n"
    printf "│%-${t_inner}s│\n" "${tlabel1}${tval1}"
    printf "│%-${t_inner}s│\n" "${tlabel2}${tval2}"
    printf "│%-${t_inner}s│\n" "${tlabel3}${tval3}"
    printf "└"; repeat_char "─" $(( t_inner )); printf "┘\n"

    # =========================================================================
    # GRAND TOTAL
    # =========================================================================
    local gpass=$(( edge_pass + rand_pass ))
    local gfail=$(( edge_fail + rand_fail ))
    echo ""
    printf "  TOTAL : %d/%d passed\n" "$gpass" "$(( gpass + gfail ))"
    if (( gfail > 0 )); then
        printf "  Failures logged to : %s\n" "$LOG_FILE"
    else
        printf "  All tests passed — no failures logged.\n"
    fi
    echo ""
}

# ------------- Entry point ---------------------------------------------------
main() {
    if [[ ! -x "$BINARY" ]]; then
        echo "Error: binary not found or not executable: $BINARY" >&2
        exit 1
    fi

    > "$LOG_FILE"

    echo "Starting PmergeMe test suite..."
    echo "  Binary     : $BINARY"
    echo "  Parallel   : $MAX_JOBS jobs"
    echo "  Rand runs  : $RUNS_PER_SIZE per size"
    echo "  Rand sizes : ${#RAND_SIZES[@]} different sizes"
    echo ""

    echo "[1/2] Running hardcoded edge cases..."
    run_edge_cases

    echo "[2/2] Running randomized tests..."
    run_random_tests

    echo "Waiting for all tests to finish..."
    wait

    print_results
}

main
