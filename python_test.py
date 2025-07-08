# Simple test without subprocess - just creates test files
import os

def create_test_files():
    """Create input and expected output files for testing"""

    # Test cases
    tests = [
        ("5 + 3 * 2", "11"),
        ("10 - 4 + 2", "8"),
        ("6 * 7", "42"),
        ("15 / 3", "5"),
        ("2 + 3 * 4", "14"),
        ("(2 + 3) * 4", "20"),
        ("10 / 2 + 3", "8"),
        ("1 + 2 * 3 + 4", "11"),
        ("20 / 4 / 2", "2.5"),
        ("3.5 + 2.5", "6")
    ]

    # Create input file
    with open("test_input.txt", "w") as f:
        for test_expr, _ in tests:
            f.write(test_expr + "\n")
        f.write("quit\n")

    # Create expected results file
    with open("expected_output.txt", "w") as f:
        f.write("Expected Results:\n")
        f.write("================\n")
        for test_expr, expected in tests:
            f.write(f"{test_expr} = {expected}\n")

    print("✓ Created test_input.txt")
    print("✓ Created expected_output.txt")
    print()
    print("HOW TO TEST:")
    print("1. Compile: g++ -o interpreter main.cpp")
    print("2. Run: ./interpreter < test_input.txt")
    print("3. Compare output with expected_output.txt")
    print()
    print("OR test manually:")
    print("1. Run: ./interpreter")
    print("2. Type each expression from test_input.txt")
    print("3. Check results match expected_output.txt")

if __name__ == "__main__":
    create_test_files()