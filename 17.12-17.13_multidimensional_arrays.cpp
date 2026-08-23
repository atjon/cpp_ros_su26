#include <iostream>
#include <array>
#include <iterator>  // for std::size
#include <vector>

// 17.12 - Multidimensional C-style arrays
// 17.13 - Multidimensional std::array

// ------------------------ 17.12 C-STYLE 2D ARRAYS ----------------------------
//
// An array can hold anything -- including another array. An "array of arrays"
// is a MULTIDIMENSIONAL array, and the usual way to model a grid.
//
//     int grid[3][4] { };     // 3 rows, each of which is an array of 4 ints
//
// Read it left to right: grid is an array of 3 things, and each of those
// things is an int[4]. The LEFT index selects the row.

// occupancy grid for a tiny robot map: 0 = free, 1 = obstacle
constexpr int kRows { 3 };
constexpr int kCols { 4 };

void printGrid(const int grid[][kCols], std::size_t rows) {
    // NOTE the signature: the FIRST dimension can be omitted (it decays), but
    // every dimension AFTER the first must be given. Without the column count
    // the compiler can't compute where row 1 starts.
    for (std::size_t r { 0 }; r < rows; ++r) {
        std::cout << "    ";
        for (std::size_t c { 0 }; c < kCols; ++c)
            std::cout << grid[r][c] << ' ';
        std::cout << '\n';
    }
}

int main() {

    std::cout << "C-style 2D array:\n";

    // initialized with nested braces, one inner list per row
    int grid[kRows][kCols] {
        { 0, 0, 1, 0 },
        { 0, 1, 1, 0 },
        { 0, 0, 0, 0 },
    };

    printGrid(grid, kRows);

    std::cout << "  grid[1][2] = " << grid[1][2] << " (row 1, column 2)\n";

    // BRACE ELISION: the inner braces can be dropped, and the values fill in
    // row by row. Legal, but far less readable -- prefer the nested form.
    // (the compiler emits -Wmissing-braces here, telling you exactly what
    //  it had to infer -- another reason to just write the braces)
    int elided[2][3] { 1, 2, 3, 4, 5, 6 };   // same as { {1,2,3}, {4,5,6} }
    std::cout << "  brace-elided elided[1][0] = " << elided[1][0] << '\n';

    // omitted values are zero-initialized, per row:
    int partial[2][3] { { 1 }, { 4, 5 } };   // -> {1,0,0}, {4,5,0}
    std::cout << "  partial[0][2] = " << partial[0][2] << " (filled in as 0)\n";

    // only the FIRST dimension may be deduced from the initializer:
    int deduced[][2] { { 1, 2 }, { 3, 4 }, { 5, 6 } };  // -> [3][2]
    std::cout << "  deduced has " << std::size(deduced) << " rows\n";
    // int bad[2][] { ... };   // ERROR if uncommented: only the first can go

    // ---------------- how it's laid out in memory -----------------------------
    //
    // A 2D array is stored in ROW-MAJOR order: all of row 0, then all of row 1,
    // and so on, in ONE contiguous block. There are no pointers involved.
    //
    //   grid[3][4] in memory:
    //   [ 0 0 1 0 | 0 1 1 0 | 0 0 0 0 ]
    //     row 0     row 1     row 2
    //
    // So grid[r][c] is really *(base + r*kCols + c).

    std::cout << "\n  memory is one flat block, row by row:\n    ";
    const int* flat { &grid[0][0] };
    for (std::size_t i { 0 }; i < kRows * kCols; ++i)
        std::cout << flat[i] << ' ';
    std::cout << '\n';

    std::cout << "  sizeof(grid) = " << sizeof(grid)
              << " bytes = " << kRows << " x " << kCols
              << " x " << sizeof(int) << '\n';

    // PERFORMANCE NOTE: because of row-major layout, looping rows-outer /
    // columns-inner walks memory in order, which is cache-friendly. Swapping
    // the loops so you stride down a column instead can be several times
    // slower on a large grid, for identical results.

    // ---------------- 17.13 MULTIDIMENSIONAL std::array -----------------------
    //
    // std::array has no built-in 2D form. You nest it -- an array of arrays:
    //
    //     std::array<std::array<int, columns>, rows>
    //
    // Note the order is INSIDE-OUT compared to the C-style version: the
    // COLUMN count is written first (it's the inner array), the ROW count
    // second. This trips up everyone at least once.

    std::cout << "\nstd::array 2D:\n";

    std::array<std::array<int, kCols>, kRows> safeGrid {{
        {{ 0, 0, 1, 0 }},
        {{ 0, 1, 1, 0 }},
        {{ 0, 0, 0, 0 }},
    }};
    // The doubled braces are because std::array is an AGGREGATE wrapping a
    // C-style array inside it, so one set of braces is for the std::array and
    // one for the inner array. Brace elision lets you write single braces in
    // most cases, but the doubled form always works.

    std::cout << "  safeGrid[1][2] = " << safeGrid[1][2] << '\n';
    std::cout << "  rows = " << safeGrid.size()
              << ", cols = " << safeGrid[0].size()
              << " <- it KNOWS its own dimensions\n";

    // and it works with range-based for, at both levels:
    std::cout << "  range-based for:\n";
    for (const auto& row : safeGrid) {
        std::cout << "    ";
        for (int cell : row) std::cout << cell << ' ';
        std::cout << '\n';
    }

    // ---------------- an alias makes it readable ------------------------------
    //
    // The nested type is a mouthful, so name it. An ALIAS TEMPLATE (13.15)
    // also lets you write the dimensions in the natural row-then-column order.

    // (declared at namespace scope in real code; shown here for proximity)
    std::cout << "\n  a template alias fixes the argument order:\n";

    // what it looks like:
    //     template <typename T, std::size_t Row, std::size_t Col>
    //     using Array2d = std::array<std::array<T, Col>, Row>;
    //
    //     Array2d<int, 3, 4> grid { };    // reads row-first, as expected

    std::cout << "    template <typename T, std::size_t Row, std::size_t Col>\n"
                 "    using Array2d = std::array<std::array<T, Col>, Row>;\n";

    // ---------------- the dynamic case ----------------------------------------
    //
    // If the dimensions aren't known until runtime, nest vectors instead.
    // Be aware that a vector-of-vectors is NOT one contiguous block -- each
    // row is separately allocated, so it's slower and more fragmented.

    std::cout << "\n  runtime dimensions, with nested vectors:\n";

    // deliberately non-const: pretend these were read from a config file or
    // a sensor at runtime, so no compile-time-sized array would work
    std::size_t rows { 3 };
    std::size_t cols { 5 };

    // outer length `rows`, each element a vector<int> of length `cols`
    std::vector<std::vector<int>> dynamicGrid(rows, std::vector<int>(cols, 0));

    dynamicGrid[2][4] = 9;

    for (const auto& row : dynamicGrid) {
        std::cout << "    ";
        for (int cell : row) std::cout << cell << ' ';
        std::cout << '\n';
    }

    // FASTER ALTERNATIVE: one flat vector plus your own indexing. This keeps
    // the contiguity (and the cache behaviour) of a real 2D array.
    std::cout << "\n  or flatten it manually -- one allocation, contiguous:\n";

    std::vector<int> flatGrid(rows * cols, 0);
    auto at { [cols](std::size_t r, std::size_t c) { return r * cols + c; } };

    flatGrid[at(1, 3)] = 7;

    for (std::size_t r { 0 }; r < rows; ++r) {
        std::cout << "    ";
        for (std::size_t c { 0 }; c < cols; ++c)
            std::cout << flatGrid[at(r, c)] << ' ';
        std::cout << '\n';
    }

    std::cout << "\n  BEST PRACTICE: nested std::array for compile-time sizes,\n"
                 "  a flat std::vector with an index helper for runtime sizes.\n"
                 "  Reach for C-style 2D arrays only when a C API demands one.\n";

    return 0;
}
