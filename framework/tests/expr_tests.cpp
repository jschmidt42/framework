/*
 * Copyright 2022-2023 - All rights reserved.
 * License: https://equals-forty-two.com/LICENSE
 */

#include <foundation/platform.h>

#if BUILD_DEVELOPMENT

#include <framework/expr.h>
#include <framework/tests/test_utils.h>

template<size_t N> FOUNDATION_FORCEINLINE expr_result_t test_expr(const char(&expr)[N], const std::initializer_list<double>& list)
{
    expr_result_t result = eval(expr, N - 1);
    CHECK(result.is_set());
    CHECK_EQ(result.element_count(), list.size());
    auto it = list.begin();
    for (auto e : result)
    {
        CHECK_EQ(e.as_number(), *it);
        ++it;
    }

    return result;
}

template<size_t N> FOUNDATION_FORCEINLINE expr_result_t test_expr(const char(&expr)[N], double expected)
{
    expr_result_t result = eval(expr, N - 1);
    CHECK_EQ(result.type, EXPR_RESULT_NUMBER);

    if (math_real_is_finite(expected))
    {
        CHECK_GE(result.value, expected - REAL_EPSILON);
        CHECK_LE(result.value, expected + REAL_EPSILON);
    }
    else
    {
        CHECK_EQ(result.value, expected);
    }

    return result;
}

template<size_t N> FOUNDATION_FORCEINLINE expr_result_t test_expr(const char(&expr)[N], int expected)
{
    expr_result_t result = eval({expr, N - 1});

    if (result.is_set())
        result = result.last();

    CHECK_EQ(result.type, EXPR_RESULT_NUMBER);
    
    const int trunc = math_trunc(result.value);
    CHECK_EQ(trunc, expected);

    return result;
}

template<size_t N> FOUNDATION_FORCEINLINE expr_result_t test_expr(const char(&expr)[N], std::nullptr_t)
{
    expr_result_t result = eval(expr, N - 1);
    CHECK_EQ(result.type, EXPR_RESULT_NULL);
    return result;
}

template<size_t N> FOUNDATION_FORCEINLINE expr_result_t test_expr(const char(&expr)[N], bool expected)
{
    expr_result_t result = eval(expr, N - 1);
    FOUNDATION_ASSERT(result.type == EXPR_RESULT_TRUE || result.type == EXPR_RESULT_FALSE || result.type == EXPR_RESULT_NULL || result.type == EXPR_RESULT_NUMBER);
    if (result.type == EXPR_RESULT_NUMBER)
        CHECK_EQ(result.value, expected ? 1.0 : 0.0);
    else if (result.type == EXPR_RESULT_NULL || result.type == EXPR_RESULT_FALSE)
        CHECK_EQ(expected, false);
    else if (result.type == EXPR_RESULT_TRUE)
        CHECK_EQ(expected, true);
    return result;
}

TEST_SUITE("Expressions")
{
    TEST_CASE("Eval Simple")
    {
        constexpr string_const_t expr = CTEXT(R"(
            1 + 2 * 3
        )");

        expr_result_t result = eval(expr);
        CHECK_EQ(result.type, EXPR_RESULT_NUMBER);
        CHECK_EQ(result.value, 7.0);
    }

    TEST_CASE("Empty")
    {
        test_expr("", nullptr);
        test_expr("  ", nullptr);
        test_expr("  \t \n ", nullptr);
    }

    TEST_CASE("Constants")
    {
        test_expr("1", 1.0);
        test_expr(" 1 ", 1.0);
        test_expr("12", 12.0);
        test_expr("123", 123.0);
        test_expr("12.3", 12.3);
        test_expr("PI", REAL_PI);
        test_expr("HALFPI", REAL_HALFPI);
        test_expr("TWOPI", REAL_TWOPI);
        test_expr("SQRT2", REAL_SQRT2);
        test_expr("SQRT3", REAL_SQRT3);
        test_expr("E", REAL_E);
        test_expr("LOGN2", REAL_LOGN2);
        test_expr("LOGN10", REAL_LOGN10);
        test_expr("EPSILON", REAL_EPSILON);
    }

    TEST_CASE("Unary")
    {
        test_expr("-1", -1);
        test_expr("--1", -(-1));
        test_expr("!0 ", !0);
        test_expr("!2 ", !2);
        test_expr("^3", ~3);
        test_expr("^-3", ~(-3));
    }

    TEST_CASE("Binary")
    {
        test_expr("1+2", 1 + 2);
        test_expr("10-2", 10 - 2);
        test_expr("2*3", 2 * 3);
        test_expr("2+3*4", 2 + 3 * 4);
        test_expr("2*3+4", 2 * 3 + 4);
        test_expr("2+3/2", 2 + 3.0 / 2.0);
        test_expr("1/3*6/4*2", 1.0 / 3 * 6 / 4.0 * 2);
        test_expr("1*3/6*4/2", 1.0 * 3 / 6 * 4.0 / 2.0);
        test_expr("6/2+8*4/2", 19);
        test_expr("3/2", 3.0 / 2.0);
        test_expr("(3/2)|0", 3 / 2);
        test_expr("(3/0)", INFINITY);
        test_expr("(3/0)|0", -2147483648i32);
        test_expr("(3%0)", NAN);
        test_expr("(3%0)|0", -2147483648i32);
        test_expr("2**3", 8);
        test_expr("9**(1/2)", 3);
        test_expr("1+2<<3", (1 + 2) << 3);
        test_expr("2<<3", 2 << 3);
        test_expr("12>>2", 12 >> 2);
        test_expr("1<2", 1 < 2);
        test_expr("2<2", 2 < 2);
        test_expr("3<2", 3 < 2);
        test_expr("1>2", 1 > 2);
        test_expr("2>2", 2 > 2);
        test_expr("3>2", 3 > 2);
        test_expr("1==2", 1 == 2);
        test_expr("2==2", 2 == 2);
        test_expr("3==2", 3 == 2);
        test_expr("3.2==3.1", 3.2f == 3.1f);
        test_expr("1<=2", 1 <= 2);
        test_expr("2<=2", 2 <= 2);
        test_expr("3<=2", 3 <= 2);
        test_expr("1>=2", 1 >= 2);
        test_expr("2>=2", 2 >= 2);
        test_expr("3>=2", 3 >= 2);
        test_expr("123&42", 123 & 42);
        test_expr("123^42", 123 ^ 42);

        test_expr("1-1+1+1", 1 - 1 + 1 + 1);
        test_expr("2**2**3", 256); /* 2^(2^3), not (2^2)^3 */
    }

    TEST_CASE("Logical")
    {
        test_expr("2&&3", 3);
        test_expr("0&&3", false);
        test_expr("3&&0", false);
        test_expr("2||3", 2);
        test_expr("0||3", 3);
        test_expr("2||0", 2);
        test_expr("0||0", false);
        test_expr("1&&2||3", 2);
        test_expr("1&&2&&3", 3);
        test_expr("1||2||3", 1);
        test_expr("1||2&&3", 1);

        test_expr("1&&(3%0)", false);
        test_expr("(3%0)&&1", NAN);
        test_expr("1||(3%0)", 1);
        test_expr("(3%0)||1", 1);
    }

    TEST_CASE("Parens")
    {
        test_expr("(1+2)*3", (1 + 2) * 3);
        test_expr("(1)", 1);
        test_expr("(2.4)", 2.4);
        test_expr("((2))", 2);
        test_expr("(((3)))", 3);
        test_expr("(((3)))*(1+(2))", 9);
        test_expr("((3))*(1+(2))", 9);
    }

    TEST_CASE("Assign")
    {
        test_expr("x=5", 5);
        test_expr("x=y=3", 3);
        test_expr("x=1+2", 3);
        test_expr("x=1+2*3", 7);
    }

    TEST_CASE("Comma")
    {
        test_expr("2,3,4", 4);
        test_expr("2+3,4*5", 4 * 5);
        test_expr("x=5, x", 5);
        test_expr("x=5, y = 3, x+y", 8);
        test_expr("x=5, x=(x!=0)", true);
        test_expr("x=5, x = x+1", 6);
    }

    TEST_CASE("Functions")
    {   
        struct nop_context {
            void* p;
        };

        expr_register_function("nop", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t
        {
            FOUNDATION_UNUSED(args);
            nop_context* nop = (nop_context*)c;
            if (f->ctxsz == 0) {
                free(nop->p);
                return NIL;
            }
            if (nop->p == NULL) {
                nop->p = malloc(10000);
            }
            return NIL;
        }, [](const expr_func_t *f, void *c)
        {
            CHECK_EQ(f->name, CTEXT("nop"));
            struct nop_context *nop = (struct nop_context *)c;
            free(nop->p);
        }, sizeof(nop_context));

        expr_register_function("next", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t
        {
            CHECK_EQ(c, nullptr);
            CHECK_EQ(f->name, CTEXT("next"));
            double a = expr_eval(args->get(0));
            return a + 1;
        });

        test_expr("add(1,2) + next(3)", 7);
        test_expr("add(1,next(2))", 4);
        test_expr("add(1,1+1) + add(2*2+1,2)", 10);
        test_expr("nop()", nullptr);
        test_expr("x=2,add(1, next(x))", 4);
        test_expr("$(zero), zero()", nullptr);
        test_expr("$(zero), zero(1, 2, 3)", nullptr);
        test_expr("$(one, 1), one()+one(1)+one(1, 2, 4)", 3);
        test_expr("$(number, 1), $(number, 2+3), number()", 5);
        test_expr("$(triw, ($1 * 256) & 255), triw(0.5, 2)", 128);
        test_expr("$(triw, ($1 * 256) & 255), triw(0.1)+triw(0.7)+triw(0.2)", 255);
        test_expr("$(sub2, sub($1, $2)), sub2(5, 3)", 2);
        test_expr("$(sub2, sub($1, $2)), sub2(5, 3)+sub2(3, 1)", 4);

        // Name collisions
        test_expr("next=5", 5);
        test_expr("next=2,next(5)+next", 8);
    }

    TEST_CASE("Auto Comma")
    {
        test_expr("a=3\na+2\n", 5);
        test_expr("a=3\n\n\na+2\n", 5);
        test_expr("\n\na=\n3\n\n\na+2\n", 5);
        test_expr("\n\n3\n\n", 3);
        test_expr("\n\n\n\n", nullptr);
        test_expr("3\n\n\n\n", 3);
        test_expr("a=3\nb=4\na", 3);
        test_expr("(\n2+3\n)\n", 5);
        test_expr("a=\n3*\n(4+\n3)\na+\na\n", 42);
    }

    TEST_CASE("Comments")
    {
        constexpr string_const_t expr = CTEXT(R"(
            # Do some maths
            mul(add(1, 2), 3) # This should return 9
        )");

        expr_result_t result = eval(expr);
        CHECK_EQ(result.type, EXPR_RESULT_NUMBER);
        CHECK_EQ(result.value, 9.0);
    }

    TEST_CASE("is_null()")
    {
        CHECK_EQ(eval("").is_null(), true);
        CHECK_EQ(eval("nil").is_null(), true);
        CHECK_EQ(eval("[true, false, true]").is_null(), false);
        CHECK_EQ(eval("[true, null, true]").is_null(0), false);
        CHECK_EQ(eval("[true, null, true]").is_null(1), true);
        CHECK_EQ(eval("[true, nil, false]").is_null(2), false);
        CHECK_EQ(eval("[true, nil, false]").is_null(5), true);

        expr_register_function("nullstring", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t { return ""; });
        CHECK_EQ(eval("nullstring()").is_null(), true);

        expr_register_function("nullptr", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            expr_result_t nil{};
            nil.type = EXPR_RESULT_POINTER;
            nil.ptr = nullptr;
            nil.index = 0;
            return nil;
        });
        CHECK_EQ(eval("nullptr()").is_null(), true);

        expr_register_function("intptr", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local int i = 42;
            return expr_result_t((void*)&i, sizeof(i)); 
        });
        CHECK_EQ(eval("intptr()").is_null(), false);
    }

    TEST_CASE("as_boolean()")
    {
        CHECK_EQ(eval("").as_boolean(), false);
        CHECK_EQ(eval("nil").as_boolean(), false);
        CHECK_EQ(eval("null").as_boolean(), false);
        CHECK_EQ(eval("0*100").as_boolean(), false);
        CHECK_EQ(eval("1*100").as_boolean(), true);
        CHECK_EQ(eval("1&&0").as_boolean(), false);
        CHECK_EQ(eval("1||0").as_boolean(), true);
        CHECK_EQ(eval("true").as_boolean(), true);
        CHECK_EQ(eval("TRUE").as_boolean(), true);
        CHECK_EQ(eval("[1 '']").as_boolean(1), false);
        CHECK_EQ(eval("false").as_boolean(), false);
        CHECK_EQ(eval("'cool cool cool'").as_boolean(), false);
        CHECK_EQ(eval("[true, false, false]").as_boolean(), true);
        CHECK_EQ(eval("[false, true, false]").as_boolean(1), true);
        CHECK_EQ(eval("[false, false, true]").as_boolean(2), true);
        CHECK_EQ(eval("[false, false, true]").as_boolean(3), false);
    }

    TEST_CASE("as_string()")
    {
        CHECK_EQ(eval("[true, false, false]").as_string(), CTEXT("[true, false, false]"));
        CHECK_EQ(eval("5+6").as_string(), CTEXT("11"));
        CHECK_EQ(eval("PI*2").as_string("%.2lf"), CTEXT("6.28"));
        CHECK_EQ(eval("NIL").as_string(), CTEXT("nil"));

        expr_register_function("ptr16u", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            static uint16_t u16[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            return expr_result_t(u16, sizeof(u16[0]), ARRAY_COUNT(u16), EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_UNSIGNED);
        });

        CHECK_EQ(eval("ptr16u()").as_string(), CTEXT("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"));

        expr_register_function("memptr", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            void* mem = memory_allocate(0, 90, 0, MEMORY_TEMPORARY);
            return expr_result_t(mem, memory_size(mem));
        });

        expr_result_t ptr = eval("memptr()");
        string_const_t str = string_format_static(STRING_CONST("0x%p (%d [%d])"), ptr.ptr, ptr.element_count(), ptr.element_size());
        CHECK_EQ(ptr.as_string(), str);
        memory_deallocate(ptr.ptr);
    }

    TEST_CASE("as_number()")
    {
        CHECK_EQ(test_expr("NIL", nullptr).as_number(0), 0);
        CHECK_EQ(eval("nil").as_number(), 0);
        CHECK_EQ(eval("null").as_number(), 0);
        CHECK_EQ(eval("invalid_should_return_default").as_number(42.0), 42.0);
        CHECK_EQ(eval("true").as_number(), 1);
        CHECK_EQ(eval("false").as_number(), 0);
        CHECK_EQ(eval("'42'").as_number(), 42.0);
        CHECK_EQ(eval("1&&0").as_number(), 0.0);
        CHECK_EQ(eval("1||0").as_number(), 1.0);

        expr_register_function("alwaystrue", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t { return true; });
        CHECK_EQ(eval("alwaystrue()").as_number(), 1.0);

        CHECK_EQ(eval("[1, 2, 3]").as_number(), 1.0);
        CHECK_EQ(eval("[1, 2, 3]").as_number(), eval("[1, 2, 3]").as_number(NAN, 0));
        CHECK_EQ(eval("[1, 2, 3]").as_number(0, 1), 2.0);
        CHECK_EQ(eval("[1, 2, 3]").as_number(0, 2), 3.0);

        CHECK_EQ(eval("[]").as_number(666.0, 0), 666.0);

        expr_register_function("emptyset", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            expr_result_t* reserved = nullptr;
            array_reserve(reserved, 32);
            return expr_eval_list(reserved); 
        });
        CHECK_EQ(eval("emptyset()").as_number(666.0, 0), 666.0);

        expr_register_function("doubles", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            static double f64[] = { 4.0 };
            return expr_result_t(f64, sizeof(double), 1, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        CHECK_EQ(eval("doubles()").as_number(0), 4.0);

        expr_register_function("emptyptr", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            static double f64[] = { 4.0 };
            return expr_result_t(f64, sizeof(double), 0, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        CHECK_EQ(eval("emptyptr()").as_number(0), 0.0);

        expr_register_function("undefinedarray", [](const expr_func_t* f, vec_expr_t* args, void* c) ->expr_result_t 
        { 
            static double f64[] = { 4.0 };
            return expr_result_t(f64, sizeof(double), 10, EXPR_POINTER_ARRAY);
        });

        CHECK_EQ(eval("undefinedarray()").as_number(0), 0.0);
    }

    TEST_CASE("is_set()")
    {
        CHECK_EQ(eval("").is_set(), false);
        CHECK_EQ(eval("nil").is_set(), false);
        CHECK_EQ(eval("null").is_set(), false);
        CHECK_EQ(eval("[null]").is_set(), true);

        expr_register_function("ptr1", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local int i = 42;
            return expr_result_t((void*)&i, sizeof(i)); 
        });

        CHECK_EQ(eval("ptr1()").is_set(), true);
    }

    TEST_CASE("element_at()")
    {
        CHECK_EQ(eval("42+42").element_at(33).as_number(), 84.0);
        CHECK_EQ(eval("[32,33]").element_at(1).as_number(), 33.0);
        CHECK_EQ(eval("[0, 32,33]").element_at(11).as_number(), NAN);

        expr_register_function("ptr2", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local int i[] = {42, 54, 66};
            return expr_result_t((void*)&i, sizeof(i[0]), 3, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_INTEGER);
        });

        CHECK_EQ(eval("ptr2()").element_at(0).as_number(), 42.0);
        CHECK_EQ(eval("ptr2()").as_number(NAN, 1), 54.0);
        CHECK_EQ(eval("ptr2()").as_number(NAN, 2), 66.0);
        CHECK_EQ(eval("ptr2()").as_number(NAN, 20), NAN);
        CHECK_EQ(eval("ptr2()").element_at(10).as_number(), NAN);

        expr_register_function("ptr64", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local int64_t i[] = {INT64_MAX, 54, 66};
            return expr_result_t((void*)&i, sizeof(i[0]), 3, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_INTEGER);
        });

        CHECK_EQ(eval("ptr64()").element_at(0).as_number(), (double)INT64_MAX);

        expr_register_function("ptru64", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local uint64_t i[] = {0, UINT32_MAX, 0, 33};
            return expr_result_t((void*)&i, sizeof(i[0]), 4, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_UNSIGNED);
        });

        CHECK_EQ(eval("ptru64()").element_count(), 4);
        CHECK_EQ(eval("ptru64()").element_size(), sizeof(uint64_t));
        CHECK_EQ(eval("ptru64()").element_at(1).as_number(), (double)UINT32_MAX);
    }

    TEST_CASE("Pointer Array")
    {
        expr_register_function("floats", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local float f32[] = {1.0f, 4.0f};
            return expr_result_t((void*)&f32, sizeof(f32[0]), 4, EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        CHECK_EQ(eval("3+6").is_raw_array(), false);
        CHECK_EQ(eval("floats()").is_raw_array(), true);

        CHECK_EQ(eval("3+6").element_size(), sizeof(real));
        CHECK_EQ(eval("0!=1").element_size(), 1);
        CHECK_EQ(eval("1==1").element_size(), 1);
        CHECK_EQ(eval("").element_size(), 0);
        CHECK_EQ(eval("infineis").element_size(), 8);
        CHECK_EQ(eval("[2,3,4]").element_size(), sizeof(real));
        CHECK_EQ(eval("[1==1,2==2,3==3]").element_size(), 1);
        CHECK_EQ(eval("floats()").element_size(), 4);

        // The returned value is transformed to a double through #element_at()
        CHECK_EQ(eval("floats()").element_at(1).element_size(), sizeof(double));
    }

    TEST_CASE("operator-")
    {
        CHECK_EQ(eval("-1").as_number(), -1.0);
        CHECK_EQ(eval("-1.0").as_number(), -1.0);
        CHECK_EQ(eval("-true").as_boolean(), false);
        CHECK_EQ(eval("-false").as_boolean(), true);
        CHECK_EQ(eval("-coucou").as_string(), CTEXT("coucou"));

        test_expr("-[1, 2, 3]", {-1, -2, -3});
    }

    TEST_CASE("operator*")
    {
        CHECK_EQ(eval("-1*88").as_number(), -88.0);
        CHECK_EQ(eval("1*88").as_number(), 88.0);
        CHECK_EQ(eval("1*88.0").as_number(), 88.0);
        CHECK_EQ(eval("1*88.0*2").as_number(), 176.0);
        CHECK_EQ(eval("true*false").as_boolean(), false);
        CHECK_EQ(eval("-(false*true)").as_boolean(), true);
        CHECK_EQ(eval("12*true").as_number(), 12.0);
        CHECK_EQ(eval("5*nil").as_number(), NAN);
        CHECK_EQ(eval("12*failure").as_number(), 0.0);
        CHECK_EQ(eval("12*'1e12'").as_number(), 12000000000000.000);

        test_expr("[12, true, -1]*4", {12*4.0, 4.0, -4.0});
        test_expr("[12, true, -1]*[4, 5, 6]", {12*4.0, 5.0, -6.0});
        test_expr("5*[12, true, -1]*[4, 5, 6, 7]", {5*12*4.0, 5*5.0, 5*-6.0});
    }

    TEST_CASE("Basic functions")
    {
        // MIN and MAX
        CHECK_EQ(eval("min(44, 55)").as_number(), 44.0);
        CHECK_EQ(eval("min(44, 55, 6)").as_number(), 6.0);
        CHECK_EQ(eval("min(7)").as_number(), 7.0);
        CHECK_EQ(eval("min(true, 7, [8, 9, 10])").as_number(), 1.0);

        expr_register_function("floats1", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local float f32[] = {1.0f, -4.0f};
            return expr_result_t((void*)&f32, sizeof(f32[0]), ARRAY_COUNT(f32), EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        expr_register_function("floats2", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local double f32[] = {2.0, 4.0};
            return expr_result_t((void*)&f32, sizeof(f32[0]), ARRAY_COUNT(f32), EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        CHECK_EQ(eval("min(floats1(), floats2())").as_number(), -4.0);

        CHECK_EQ(eval("max(44, min(floats2(), [null, nan]), [null, 99, -1000], 5-5)").as_number(), 99.0);

        CHECK_EQ(eval("min()==max()").as_boolean(), true);

        // SUM
        CHECK_EQ(eval("sum(min([4, 2, 6]), 77, 88)==sum(77, 88, 2)").as_boolean(), true);
        CHECK_EQ(eval("sum(true, false, null, [0, 0])").as_number(), 1.0);
        CHECK_EQ(eval("sum(floats2(), floats1())").as_number(), 3.0);

        expr_register_function("small_numbers", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local double n[] = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4};
            return expr_result_t((void*)&n, sizeof(n[0]), ARRAY_COUNT(n), EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_FLOAT);
        });

        CHECK_EQ(eval("sum(small_numbers()/2.0)<1").as_boolean(), true);

        expr_register_function("large_numbers", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            static thread_local int32_t n[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
            return expr_result_t((void*)&n, sizeof(n[0]), ARRAY_COUNT(n), EXPR_POINTER_ARRAY | EXPR_POINTER_ARRAY_INTEGER);
        });

        CHECK_EQ(eval("sum(small_numbers())").as_number(), 0.0055);
        CHECK_EQ(eval("sum(small_numbers())<sum(large_numbers())").as_boolean(), true);
        CHECK_EQ(eval("sum()").as_string(), CTEXT("nil"));

        // AVG
        CHECK(eval("avg()==null"));
        CHECK(eval("[avg(1,2,3)]==[sum(1,1)]"));
        CHECK_EQ(eval("avg(1, 2, 3, 4, 5)").as_number(), 3.0);
        CHECK_EQ(eval("avg(large_numbers())").as_number(), 5500.0);
        CHECK_EQ(eval("avg([1,1,1],sum(1),null)").as_number(), 1.0);

        // COUNT
        CHECK_EQ(eval("count(1, 2, 3, 4, 5)").as_number(), 5.0);
        CHECK_EQ(eval("count(large_numbers(), 2, 3, 4, 5, 6, 7, 8, 9, 10)").as_number(), 19);
    }

    TEST_CASE("IF")
    {
        expr_register_function("func", [](const expr_func_t* f, vec_expr_t* args, void* c) -> expr_result_t 
        { 
            return expr_result_t(6.0);
        });

        test_expr("if(func()>5, true, false)", true);
        test_expr("if(func()<5, true)", nullptr);
        test_expr("if(func()<5, true, add(1, 2))", 3.0);
    }

    TEST_CASE("WHILE")
    {
        test_expr("i=0, s=0, $(inc, $1+1), while((i=inc(i))<6, s=sum(s, 1))", 5);
        test_expr("i=0, s=0, $(inc, $1+1), while((i=inc(i))<6, s=sum(s, 2))", 10);
    }
}

#endif // BUILD_DEVELOPMENT
