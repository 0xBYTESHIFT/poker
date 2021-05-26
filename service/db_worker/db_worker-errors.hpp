#pragma once

enum class db_worker_error{
    ok = 0,
    stl_error,
    sql_error,
    unknown_error,
    SIZE;
};