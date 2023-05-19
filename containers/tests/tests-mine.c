/*
 * You can use this file for your own tests
 */

#include "libs/cut.h"
#include "libs/mainwrap.h"
#include "libs/utils.h"

#include <stdlib.h>

#define CONTAINERS_FILE "../tests/data/example-containers.csv"
#define PATHS_FILE "../tests/data/example-paths.csv"

/* The following “extentions” to CUT are available in this test file:
 *
 * • ‹CHECK_IS_EMPTY(file)› — test whether the file is empty.
 * • ‹CHECK_NOT_EMPTY(file)› — inverse of the above.
 *
 * • ‹app_main_args(ARG…)› — call your ‹main()› with given arguments.
 * • ‹app_main()› — call your ‹main()› without any arguments. */
/*
TEST(my_test)
{
    int rv = 0; // return value of main()
    CHECK(app_main_args("FILE") == rv); //arguments for main()

    const char *correct_output =
        "Output on STDOUT"
    ;

    ASSERT_FILE(stdout, correct_output);
    ASSERT_FILE(stderr, "");
}
*/

TEST(no_argument)
{
    CHECK(app_main() == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(exclusive_options)
{
    CHECK(app_main_args("-t", "A", "-s", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(duplicate_options)
{
    CHECK(app_main_args("-t", "A", "-t", "AP", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(missing_parameter)
{
    CHECK(app_main_args("-t", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(missing_filename)
{
    CHECK(app_main_args("-t", "A", "data/example-paths.csv") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(invalid_parameter)
{
    CHECK(app_main_args("-t", "Aa", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(invalid_parameter_2)
{
    CHECK(app_main_args("-c", "100,5", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(invalid_parameter_3)
{
    CHECK(app_main_args("-c", "100-a5", CONTAINERS_FILE, PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Usage: app [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n");
}

TEST(missing_id_paths)
{
    CHECK(app_main_args(CONTAINERS_FILE, "../tests/data/missing_id_paths.csv") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid path format at line 3\n");
}

TEST(invalid_number_format_paths)
{
    CHECK(app_main_args(CONTAINERS_FILE, "../tests/data/invalid_number_format_paths.csv") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid path format at line 3\n");
}

TEST(overflow_id_paths)
{
    CHECK(app_main_args(CONTAINERS_FILE, "../tests/data/overflow_id_paths.csv") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid path format at line 3\n");
}

TEST(duplicate_id_containers)
{
    CHECK(app_main_args("../tests/data/duplicate_id_containers.csv", PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Duplicate ID at line 3\n");
}

TEST(without_optional_parameters_containers)
{
    CHECK(app_main_args("../tests/data/without_optional_parameters_containers.csv", PATHS_FILE) == EXIT_SUCCESS);

    const char *correct_output =
            "ID: 1, Type: Colored glass, Capacity: 1550, Address:, Neighbors: 4\n"
            "ID: 2, Type: Clear glass, Capacity: 1550, Address: Drozdi 55, Neighbors: 4\n"
            "ID: 3, Type: Plastics and Aluminium, Capacity: 1100, Address: Drozdi 55, Neighbors: 4\n"
            "ID: 4, Type: Colored glass, Capacity: 900, Address: Drozdi 55, Neighbors: 1 2 3 5 8\n"
            "ID: 5, Type: Paper, Capacity: 5000, Address: Klimesova 60, Neighbors: 4 8\n"
            "ID: 6, Type: Colored glass, Capacity: 3000, Address: Klimesova 60, Neighbors: 8\n"
            "ID: 7, Type: Plastics and Aluminium, Capacity: 5000, Address: Klimesova 60, Neighbors: 8\n"
            "ID: 8, Type: Biodegradable waste, Capacity: 3000, Address: Na Buble 5, Neighbors: 4 5 6 7 11\n"
            "ID: 9, Type: Textile, Capacity: 500, Address: Na Buble 5, Neighbors: 10\n"
            "ID: 10, Type: Plastics and Aluminium, Capacity: 900, Address: Odlehla 70, Neighbors: 9\n"
            "ID: 11, Type: Paper, Capacity: 2000, Address: Odlehla 70, Neighbors: 8\n"
    ;

    ASSERT_FILE(stdout, correct_output);
}

TEST(invalid_decimal_number_containers)
{
    CHECK(app_main_args("../tests/data/invalid_decimal_number_containers.csv", PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid container format at line 3\n");
}

TEST(long_decimal_number_container)
{
    CHECK(app_main_args("../tests/data/long_decimal_number_container.csv", PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Invalid container format at line 3\n");
}

TEST(id_not_in_containers_file)
{
    CHECK(app_main_args(CONTAINERS_FILE, "../tests/data/id_not_in_containers_file.csv") == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Paths file contains id which is not in containers file\n");
}

TEST(empty_paths)
{
    CHECK(app_main_args(CONTAINERS_FILE, "../tests/data/empty_paths.csv") == EXIT_SUCCESS);

    const char *correct_output =
            "ID: 1, Type: Colored glass, Capacity: 1550, Address: Drozdi 55, Neighbors:\n"
            "ID: 2, Type: Clear glass, Capacity: 1550, Address: Drozdi 55, Neighbors:\n"
            "ID: 3, Type: Plastics and Aluminium, Capacity: 1100, Address: Drozdi 55, Neighbors:\n"
            "ID: 4, Type: Colored glass, Capacity: 900, Address: Drozdi 55, Neighbors:\n"
            "ID: 5, Type: Paper, Capacity: 5000, Address: Klimesova 60, Neighbors:\n"
            "ID: 6, Type: Colored glass, Capacity: 3000, Address: Klimesova 60, Neighbors:\n"
            "ID: 7, Type: Plastics and Aluminium, Capacity: 5000, Address: Klimesova 60, Neighbors:\n"
            "ID: 8, Type: Biodegradable waste, Capacity: 3000, Address: Na Buble 5, Neighbors:\n"
            "ID: 9, Type: Textile, Capacity: 500, Address: Na Buble 5, Neighbors:\n"
            "ID: 10, Type: Plastics and Aluminium, Capacity: 900, Address: Odlehla 70, Neighbors:\n"
            "ID: 11, Type: Paper, Capacity: 2000, Address: Odlehla 70, Neighbors:\n"
    ;

    ASSERT_FILE(stdout, correct_output);
}

TEST(empty_containers_1)
{
    CHECK(app_main_args("../tests/data/empty_containers.csv", "../tests/data/empty_containers.csv") == EXIT_SUCCESS);
    ASSERT_FILE(stdout, "");
}

TEST(empty_containers_2)
{
    CHECK(app_main_args("../tests/data/empty_containers.csv", PATHS_FILE) == EXIT_FAILURE);
    ASSERT_FILE(stderr, "Paths file contains id which is not in containers file\n");
}

TEST(shortest_path)
{
    CHECK(app_main_args("-g", "1,5", CONTAINERS_FILE, PATHS_FILE) == EXIT_SUCCESS);
    ASSERT_FILE(stdout, "1-2-3-4-5 1300\n");
}

TEST(different_length_coords)
{
    CHECK(app_main_args("-s", "../tests/data/different_length_coords.csv", PATHS_FILE) == EXIT_SUCCESS);

    const char *correct_output =
            "1;AGC;2\n"
            "2;C;1,3,4\n"
            "3;APC;2,4\n"
            "4;BT;2,3,5\n"
            "5;AP;4\n"
    ;

    ASSERT_FILE(stdout, correct_output);
}

