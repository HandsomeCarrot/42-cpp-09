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

# ------------- Core test runner ----------------------------------------------
# run_test  out_file  type  label  num [num ...]
# type is "edge" or "rand" — written as first field so the parent can sort results
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
# _PIDS tracks only active pids for throttling.
# Result files are discovered by globbing TMPDIR_ROOT at the end —
# this avoids the bug where throttle_jobs pruned completed entries from _FILES.
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

    # Filename computed in parent before fork
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
            # Generate numbers in parent shell, then pass to launch_test
            nums_str=$(shuf -i 1-1000000 -n "$size" | tr '\n' ' ')
            # shellcheck disable=SC2086
            launch_test rand "size=${size} run=${run}" $nums_str
        done
    done
}

# ------------- Display -------------------------------------------------------

print_results() {
    local edge_files=() rand_files=()
    local f type

    # Discover all result files by glob — never relies on an in-memory list
    # that throttle_jobs could have pruned.
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
    echo "╔══════════════════════════════════════════════════════════╗"
    echo "║              PmergeMe  Test  Results                    ║"
    echo "╚══════════════════════════════════════════════════════════╝"

    # ---- Edge cases ---------------------------------------------------------
    echo ""
    echo "┌─ HARDCODED EDGE CASES ──────────────────────────────────────────────────────────┐"
    printf "│ %-25s │ %-4s │ %8s │ %8s │ %10s │ %10s │\n" \
        "Label" "OK?" "cmpV/max" "cmpD/max" "T_vec(us)" "T_deq(us)"
    echo "│─────────────────────────────────────────────────────────────────────────────────│"

    local edge_pass=0 edge_fail=0
    for f in "${edge_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r _t label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local cv_d="${cmp_vec}/${cmp_max}"
        local cd_d="${cmp_deq}/${cmp_max}"
        local ok="PASS"
        [[ "$status" == "FAIL" ]] && ok="FAIL"
        printf "│ %-25s │ %-4s │ %8s │ %8s │ %10s │ %10s │\n" \
            "$label" "$ok" "$cv_d" "$cd_d" "$t_vec" "$t_deq"
        if [[ "$status" == "FAIL" ]]; then
            printf "│  ↳ %-78s│\n" "$reasons"
            (( edge_fail++ ))
        else
            (( edge_pass++ ))
        fi
    done
    echo "└─────────────────────────────────────────────────────────────────────────────────┘"
    printf "  Edge cases : %d passed, %d failed\n" "$edge_pass" "$edge_fail"

    # ---- Randomized ---------------------------------------------------------
    echo ""
    echo "┌─ RANDOMIZED TESTS ──────────────────────────────────────────────────────────────────────┐"
    printf "│ %6s │ %4s │ %5s │ %5s │ %8s │ %8s │ %12s │ %12s │ %-6s │\n" \
        "Size" "Runs" "Vec✓" "Deq✓" "AvgCmpV" "AvgCmpD" "AvgT_vec(us)" "AvgT_deq(us)" "Faster"
    echo "│─────────────────────────────────────────────────────────────────────────────────────────│"

    local rand_pass=0 rand_fail=0
    declare -A sz_runs sz_vpass sz_dpass sz_fail
    declare -A sz_scv sz_scd sz_stv sz_std

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

        [[ "$status" == "PASS" ]] && (( rand_pass++ )) || (( rand_fail++ ))
    done

    local total_tv=0 total_td=0 total_rand=0
    for sz in "${RAND_SIZES[@]}"; do
        local runs=${sz_runs[$sz]:-0}
        (( runs == 0 )) && continue
        local vp=${sz_vpass[$sz]:-0}
        local dp=${sz_dpass[$sz]:-0}
        local fc=${sz_fail[$sz]:-0}
        local acv acd atv atd
        acv=$(avg_float "${sz_scv[$sz]:-0}" "$runs")
        acd=$(avg_float "${sz_scd[$sz]:-0}" "$runs")
        atv=$(avg_float "${sz_stv[$sz]:-0}" "$runs")
        atd=$(avg_float "${sz_std[$sz]:-0}" "$runs")

        local faster="tie   "
        (( ${sz_stv[$sz]:-0} < ${sz_std[$sz]:-0} )) && faster="vector"
        (( ${sz_std[$sz]:-0} < ${sz_stv[$sz]:-0} )) && faster="deque "

        local flag=""
        (( fc > 0 )) && flag="  !"

        printf "│ %6s │ %4s │ %5s │ %5s │ %8s │ %8s │ %12s │ %12s │ %-6s │%s\n" \
            "$sz" "$runs" "$vp/$runs" "$dp/$runs" \
            "$acv" "$acd" "$atv" "$atd" \
            "$faster" "$flag"

        total_tv=$(( total_tv + ${sz_stv[$sz]:-0} ))
        total_td=$(( total_td + ${sz_std[$sz]:-0} ))
        total_rand=$(( total_rand + runs ))
    done
    echo "└─────────────────────────────────────────────────────────────────────────────────────────┘"
    printf "  Randomized : %d passed, %d failed\n" "$rand_pass" "$rand_fail"

    # ---- Timing summary -----------------------------------------------------
    echo ""
    echo "┌─ TIMING SUMMARY ────────────────────────────────────┐"
    if (( total_rand > 0 )); then
        local avg_tv avg_td
        avg_tv=$(avg_float "$total_tv" "$total_rand")
        avg_td=$(avg_float "$total_td" "$total_rand")
        printf "│  Avg T_vec : %14s us                    │\n" "$avg_tv"
        printf "│  Avg T_deq : %14s us                    │\n" "$avg_td"
        if   (( total_tv < total_td )); then
            printf "│  Faster    : vector                              │\n"
        elif (( total_td < total_tv )); then
            printf "│  Faster    : deque                               │\n"
        else
            printf "│  Faster    : tied                                │\n"
        fi
    fi
    echo "└─────────────────────────────────────────────────────┘"

    # ---- Grand total --------------------------------------------------------
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
