#!/usr/bin/env bash
# =============================================================================
# test_pmergeme.sh — Test harness for PmergeMe (cpp09 ex02)
# =============================================================================

# ------------- Configuration -------------------------------------------------
BINARY="./PmergeMe"
MAX_JOBS=8                  # parallel workers
RUNS_PER_SIZE=5             # randomized runs per input size
LOG_FILE="pmergeme_failures.log"

# Randomized test sizes
# Includes 2-20 sequentially, then powers-of-2 boundaries and Jacobsthal-
# relevant sizes where Ford-Johnson edge cases are most likely to appear.
RAND_SIZES=(
    2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
    25 32 33 50 64 65 100 128 129 256 257 500 512 513
    1000 2000 3000
)

# ------------- Temp workspace ------------------------------------------------
TMPDIR_ROOT="$(mktemp -d /tmp/pmergeme_test.XXXXXX)"
trap 'rm -rf "$TMPDIR_ROOT"' EXIT

# ------------- Helpers -------------------------------------------------------

# Convert "0 s 0 ms 708 us" → integer microseconds
parse_time_us() {
    local line="$1"
    # extract the three numbers after the colon
    local time_part
    time_part="$(echo "$line" | sed 's/.*: //')"
    local s ms us
    s=$(echo  "$time_part" | awk '{print $1}')
    ms=$(echo "$time_part" | awk '{print $3}')
    us=$(echo "$time_part" | awk '{print $5}')
    echo $(( s * 1000000 + ms * 1000 + us ))
}

# Run one test, write result to $out_file
# Arguments: out_file label input_numbers...
run_test() {
    local out_file="$1"
    local label="$2"
    shift 2
    local input=("$@")

    # Capture output; redirect stderr to stdout so we catch error messages too
    local raw
    raw="$("$BINARY" "${input[@]}" 2>&1)"
    local exit_code=$?

    # Extract the 4 relevant lines from the tail
    local sorted_line cmp_line tvec_line tdeq_line
    sorted_line=$(echo "$raw" | grep '^sorted:')
    cmp_line=$(echo    "$raw" | grep '^comparisons:')
    tvec_line=$(echo   "$raw" | grep 'std::vector')
    tdeq_line=$(echo   "$raw" | grep 'std::deque')

    # ---- Parse sorted -------------------------------------------------------
    local vec_sorted deq_sorted
    vec_sorted=$(echo "$sorted_line" | grep -o 'vector = [a-z]*' | awk '{print $3}')
    deq_sorted=$(echo "$sorted_line" | grep -o 'deque = [a-z]*'  | awk '{print $3}')

    # ---- Parse comparisons --------------------------------------------------
    local cmp_max cmp_vec cmp_deq
    cmp_max=$(echo "$cmp_line" | grep -o 'max = [0-9]*'    | awk '{print $3}')
    cmp_vec=$(echo "$cmp_line" | grep -o 'vector = [0-9]*' | awk '{print $3}')
    cmp_deq=$(echo "$cmp_line" | grep -o 'deque = [0-9]*'  | awk '{print $3}')

    # ---- Parse timing -------------------------------------------------------
    local t_vec_us t_deq_us
    t_vec_us=$(parse_time_us "$tvec_line")
    t_deq_us=$(parse_time_us "$tdeq_line")

    # ---- Determine failure reasons ------------------------------------------
    local fail_reasons=""

    # Program crashed / returned error
    if [[ $exit_code -ne 0 ]]; then
        fail_reasons="exit_code=$exit_code"
    fi

    # Missing expected output lines (program may have errored out early)
    if [[ -z "$sorted_line" || -z "$cmp_line" || -z "$tvec_line" || -z "$tdeq_line" ]]; then
        fail_reasons="${fail_reasons:+$fail_reasons, }missing_output_lines"
    fi

    # sorted check
    if [[ "$vec_sorted" != "true" ]]; then
        fail_reasons="${fail_reasons:+$fail_reasons, }vector_not_sorted"
    fi
    if [[ "$deq_sorted" != "true" ]]; then
        fail_reasons="${fail_reasons:+$fail_reasons, }deque_not_sorted"
    fi

    # comparisons exceed maximum
    if [[ -n "$cmp_vec" && -n "$cmp_max" ]] && (( cmp_vec > cmp_max )); then
        fail_reasons="${fail_reasons:+$fail_reasons, }vec_cmp_exceeded(${cmp_vec}>${cmp_max})"
    fi
    if [[ -n "$cmp_deq" && -n "$cmp_max" ]] && (( cmp_deq > cmp_max )); then
        fail_reasons="${fail_reasons:+$fail_reasons, }deq_cmp_exceeded(${cmp_deq}>${cmp_max})"
    fi

    local status="PASS"
    [[ -n "$fail_reasons" ]] && status="FAIL"

    # Write structured result record (tab-separated) to temp file
    printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
        "$label" "$status" \
        "${vec_sorted:-?}" "${deq_sorted:-?}" \
        "${cmp_max:-?}" "${cmp_vec:-?}" "${cmp_deq:-?}" \
        "${t_vec_us:-0}" "${t_deq_us:-0}" \
        "${fail_reasons}" \
        > "$out_file"

    # On failure, also append to log (with input and raw output)
    if [[ "$status" == "FAIL" ]]; then
        {
            echo "========================================"
            echo "LABEL  : $label"
            echo "INPUT  : ${input[*]}"
            echo "REASONS: $fail_reasons"
            echo "--- relevant output ---"
            echo "$sorted_line"
            echo "$cmp_line"
            echo "$tvec_line"
            echo "$tdeq_line"
            echo "--- full output ---"
            echo "$raw"
            echo ""
        } >> "$LOG_FILE"
    fi
}

# ------------- Job pool ------------------------------------------------------
declare -a JOB_PIDS=()
declare -a JOB_FILES=()

# Wait until fewer than MAX_JOBS background jobs are running
throttle_jobs() {
    while (( ${#JOB_PIDS[@]} >= MAX_JOBS )); do
        local new_pids=()
        local new_files=()
        local i
        for i in "${!JOB_PIDS[@]}"; do
            if kill -0 "${JOB_PIDS[$i]}" 2>/dev/null; then
                new_pids+=("${JOB_PIDS[$i]}")
                new_files+=("${JOB_FILES[$i]}")
            fi
        done
        JOB_PIDS=("${new_pids[@]}")
        JOB_FILES=("${new_files[@]}")
        (( ${#JOB_PIDS[@]} >= MAX_JOBS )) && sleep 0.05
    done
}

# Launch one test in the background
launch_test() {
    local label="$1"; shift
    local out_file="$TMPDIR_ROOT/$(echo "$label" | tr ' /' '__').result"
    throttle_jobs
    run_test "$out_file" "$label" "$@" &
    JOB_PIDS+=($!)
    JOB_FILES+=("$out_file")
}

# ------------- Hardcoded edge cases ------------------------------------------
run_edge_cases() {
    # 2 numbers
    launch_test "edge: already sorted (2)"       1 2
    launch_test "edge: reverse sorted (2)"       2 1
    launch_test "edge: equal pair"               7 7

    # small sequences
    launch_test "edge: already sorted (5)"       1 2 3 4 5
    launch_test "edge: reverse sorted (5)"       5 4 3 2 1
    launch_test "edge: all equal (5)"            3 3 3 3 3
    launch_test "edge: single swap needed"       1 3 2

    # power-of-2 boundaries (Ford-Johnson Jacobsthal groups)
    launch_test "edge: size 3 (2+1)"             3 1 2
    launch_test "edge: size 4"                   4 3 2 1
    launch_test "edge: size 5 (4+1)"             5 1 4 2 3
    launch_test "edge: size 8"                   8 7 6 5 4 3 2 1
    launch_test "edge: size 9 (8+1)"             9 1 8 2 7 3 6 4 5
    launch_test "edge: size 16"                  16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
    launch_test "edge: size 17 (16+1)"           17 1 16 2 15 3 14 4 13 5 12 6 11 7 10 8 9

    # large sorted / reverse-sorted (stress the recursion)
    local sorted_3000=( $(seq 1 3000) )
    local rev_3000=(    $(seq 3000 -1 1) )
    launch_test "edge: sorted 3000"   "${sorted_3000[@]}"
    launch_test "edge: reversed 3000" "${rev_3000[@]}"
}

# ------------- Randomized tests ----------------------------------------------
run_random_tests() {
    local size run seq_args
    for size in "${RAND_SIZES[@]}"; do
        for (( run=1; run<=RUNS_PER_SIZE; run++ )); do
            # shuf picks 'size' unique numbers from 1..1000000
            # For sizes > 1000000 this would fail, but our max is 3000
            mapfile -t seq_args < <(shuf -i 1-1000000 -n "$size")
            launch_test "rand: size=${size} run=${run}" "${seq_args[@]}"
        done
    done
}

# ------------- Results collection & display ----------------------------------

# bc helper — returns empty string if bc unavailable, falls back to awk
avg_us() {
    # $1 = sum, $2 = count
    if (( $2 == 0 )); then echo "0"; return; fi
    awk "BEGIN { printf \"%.1f\", $1 / $2 }"
}

print_results() {
    # Separate edge and rand result files
    local edge_files=() rand_files=()
    local f
    for f in "${JOB_FILES[@]}"; do
        [[ -f "$f" ]] || continue
        local label
        label=$(cut -f1 "$f")
        if [[ "$label" == edge:* ]]; then
            edge_files+=("$f")
        else
            rand_files+=("$f")
        fi
    done

    # -------------------------------------------------------------------------
    echo ""
    echo "╔══════════════════════════════════════════════════════════════════════╗"
    echo "║                     PmergeMe Test Results                           ║"
    echo "╚══════════════════════════════════════════════════════════════════════╝"

    # ---- Edge cases ---------------------------------------------------------
    echo ""
    echo "┌─ HARDCODED EDGE CASES ─────────────────────────────────────────────┐"
    printf "│ %-35s │ %6s │ %5s │ %5s │ %9s │ %9s │\n" \
        "Label" "Status" "cmpV" "cmpD" "T_vec(us)" "T_deq(us)"
    echo "│─────────────────────────────────────────────────────────────────────│"

    local edge_pass=0 edge_fail=0
    for f in "${edge_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local short_label="${label#edge: }"
        local status_disp="$status"
        [[ "$status" == "PASS" ]] && (( edge_pass++ )) || (( edge_fail++ ))
        printf "│ %-35s │ %6s │ %5s │ %5s │ %9s │ %9s │\n" \
            "$short_label" "$status_disp" \
            "${cmp_vec}/${cmp_max}" "${cmp_deq}/${cmp_max}" \
            "$t_vec" "$t_deq"
        if [[ "$status" == "FAIL" ]]; then
            printf "│   %-67s │\n" "  ↳ $reasons"
        fi
    done
    echo "└─────────────────────────────────────────────────────────────────────┘"
    echo "  Edge cases: ${edge_pass} passed, ${edge_fail} failed"

    # ---- Randomized tests ---------------------------------------------------
    echo ""
    echo "┌─ RANDOMIZED TESTS ─────────────────────────────────────────────────────────────────────────────┐"
    printf "│ %6s │ %5s │ %4s │ %4s │ %8s │ %8s │ %12s │ %12s │ %s\n" \
        "Size" "Runs" "VecP" "DeqP" "AvgCmpV" "AvgCmpD" "AvgT_vec(us)" "AvgT_deq(us)" "Faster"
    echo "│────────────────────────────────────────────────────────────────────────────────────────────────│"

    local rand_pass=0 rand_fail=0
    local prev_size=""

    # Collect per-size stats; process sizes in definition order
    declare -A size_runs size_vec_pass size_deq_pass size_fail_count
    declare -A size_sum_cmp_vec size_sum_cmp_deq
    declare -A size_sum_t_vec size_sum_t_deq

    for f in "${rand_files[@]}"; do
        [[ -f "$f" ]] || continue
        IFS=$'\t' read -r label status vec_s deq_s cmp_max cmp_vec cmp_deq t_vec t_deq reasons < "$f"
        local sz
        sz=$(echo "$label" | grep -o 'size=[0-9]*' | cut -d= -f2)
        [[ -z "$sz" ]] && continue

        size_runs[$sz]=$(( ${size_runs[$sz]:-0} + 1 ))
        [[ "$vec_s" == "true" ]] && size_vec_pass[$sz]=$(( ${size_vec_pass[$sz]:-0} + 1 ))
        [[ "$deq_s" == "true" ]] && size_deq_pass[$sz]=$(( ${size_deq_pass[$sz]:-0} + 1 ))
        [[ "$status" == "FAIL" ]] && size_fail_count[$sz]=$(( ${size_fail_count[$sz]:-0} + 1 ))

        # only accumulate numeric values
        if [[ "$cmp_vec" =~ ^[0-9]+$ ]]; then
            size_sum_cmp_vec[$sz]=$(( ${size_sum_cmp_vec[$sz]:-0} + cmp_vec ))
        fi
        if [[ "$cmp_deq" =~ ^[0-9]+$ ]]; then
            size_sum_cmp_deq[$sz]=$(( ${size_sum_cmp_deq[$sz]:-0} + cmp_deq ))
        fi
        if [[ "$t_vec" =~ ^[0-9]+$ ]]; then
            size_sum_t_vec[$sz]=$(( ${size_sum_t_vec[$sz]:-0} + t_vec ))
        fi
        if [[ "$t_deq" =~ ^[0-9]+$ ]]; then
            size_sum_t_deq[$sz]=$(( ${size_sum_t_deq[$sz]:-0} + t_deq ))
        fi

        [[ "$status" == "PASS" ]] && (( rand_pass++ )) || (( rand_fail++ ))
    done

    # Print in the same order as RAND_SIZES
    local total_t_vec=0 total_t_deq=0 total_runs=0
    for sz in "${RAND_SIZES[@]}"; do
        local runs=${size_runs[$sz]:-0}
        (( runs == 0 )) && continue
        local vp=${size_vec_pass[$sz]:-0}
        local dp=${size_deq_pass[$sz]:-0}
        local fc=${size_fail_count[$sz]:-0}
        local avg_cv avg_cd avg_tv avg_td
        avg_cv=$(avg_us "${size_sum_cmp_vec[$sz]:-0}" "$runs")
        avg_cd=$(avg_us "${size_sum_cmp_deq[$sz]:-0}" "$runs")
        avg_tv=$(avg_us "${size_sum_t_vec[$sz]:-0}" "$runs")
        avg_td=$(avg_us "${size_sum_t_deq[$sz]:-0}" "$runs")

        # which container is faster on average
        local faster="  tie"
        local sv=${size_sum_t_vec[$sz]:-0}
        local sd=${size_sum_t_deq[$sz]:-0}
        if   (( sv < sd )); then faster="vector"
        elif (( sd < sv )); then faster="deque "
        fi

        local fail_mark=""
        (( fc > 0 )) && fail_mark=" !"

        printf "│ %6s │ %5s │ %4s │ %4s │ %8s │ %8s │ %12s │ %12s │ %s%s\n" \
            "$sz" "$runs" "$vp/$runs" "$dp/$runs" \
            "$avg_cv" "$avg_cd" "$avg_tv" "$avg_td" \
            "$faster" "$fail_mark"

        total_t_vec=$(( total_t_vec + ${size_sum_t_vec[$sz]:-0} ))
        total_t_deq=$(( total_t_deq + ${size_sum_t_deq[$sz]:-0} ))
        total_runs=$(( total_runs + runs ))
    done
    echo "└────────────────────────────────────────────────────────────────────────────────────────────────┘"
    echo "  Randomized: ${rand_pass} passed, ${rand_fail} failed"

    # ---- Overall timing winner ----------------------------------------------
    echo ""
    echo "┌─ TIMING SUMMARY ───────────────────────────────────────────────────┐"
    if (( total_runs > 0 )); then
        local overall_avg_vec overall_avg_deq
        overall_avg_vec=$(avg_us "$total_t_vec" "$total_runs")
        overall_avg_deq=$(avg_us "$total_t_deq" "$total_runs")
        printf "│  Average T_vec across all rand tests : %10s us              │\n" "$overall_avg_vec"
        printf "│  Average T_deq across all rand tests : %10s us              │\n" "$overall_avg_deq"
        if   (( total_t_vec < total_t_deq )); then
            echo "│  Overall faster container: vector                                   │"
        elif (( total_t_deq < total_t_vec )); then
            echo "│  Overall faster container: deque                                    │"
        else
            echo "│  Overall: containers tied on average                                │"
        fi
    fi
    echo "└─────────────────────────────────────────────────────────────────────┘"

    # ---- Grand total --------------------------------------------------------
    local grand_pass=$(( edge_pass + rand_pass ))
    local grand_fail=$(( edge_fail + rand_fail ))
    local grand_total=$(( grand_pass + grand_fail ))
    echo ""
    echo "  TOTAL: ${grand_pass}/${grand_total} passed"
    if (( grand_fail > 0 )); then
        echo "  Failures logged to: $LOG_FILE"
    else
        echo "  All tests passed — no failures logged."
    fi
    echo ""
}

# ------------- Entry point ---------------------------------------------------
main() {
    # Validate binary exists
    if [[ ! -x "$BINARY" ]]; then
        echo "Error: binary not found or not executable: $BINARY" >&2
        echo "Run from the directory containing PmergeMe, or adjust BINARY at the top of this script." >&2
        exit 1
    fi

    # Fresh log file
    > "$LOG_FILE"

    echo "Starting PmergeMe test suite..."
    echo "  Binary     : $BINARY"
    echo "  Parallel   : $MAX_JOBS jobs"
    echo "  Rand runs  : ${RUNS_PER_SIZE} per size"
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
