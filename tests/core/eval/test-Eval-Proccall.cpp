#include <gtest/gtest.h>

#include "core/base/Core.h"
#include "core/eval/Proccall_impl.h"
#include "core/eval/Define_impl.h"
#include "core/eval/Eval.h"

#include <sstream>

using namespace shaka;

TEST(Proccall, initialization) {
    using namespace shaka;
    auto lambda_list = std::make_shared<DataNode>(DataNode::list(
        Symbol("lambda"),
        DataNode::list(Symbol("x")),
        Symbol("x")
    ));

    auto define_expr = std::make_shared<DataNode>(DataNode::list(
        Symbol("define"),
        Symbol("asdf")
    ));
    define_expr->append(*lambda_list);

    auto list = std::make_shared<DataNode>(DataNode::list(
        Symbol("define"),
        Symbol("asdf"),
	    DataNode(Number(1))
    ));

}

TEST(Proccall, multi_argument_lambda) {
	using namespace shaka;
	auto lambda_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(Symbol("lambda")),
		DataNode::list(DataNode(Symbol("x")), DataNode(Symbol("y"))),
		DataNode::list(DataNode(Symbol("+")), DataNode(Symbol("x")), DataNode(Symbol("y")))
	));

	std::cout << *lambda_list << std::endl;	

	EnvPtr env = std::make_shared<shaka::Environment>(nullptr);
	env->set_value(Symbol("+"), make_node(NativeProcedure(stdproc::add, 1, true)));
	
	Evaluator lambda_evaluator(lambda_list->cdr(), env);
	
	auto proc = lambda_evaluator.evaluate(eval::Lambda());
	
	ASSERT_EQ(proc->is_procedure(), true);
	ASSERT_EQ(get<Procedure>(proc->get_data()).get_fixed_arity(), 2);
	ASSERT_EQ(get<Procedure>(proc->get_data()).is_variable_arity(), false);

	auto procedure_application_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(proc),
		DataNode(Number(1)),
		DataNode(Number(2))
	));

	std::cout << *procedure_application_list << std::endl;

	Evaluator proccall_evaluator(procedure_application_list, env);

	auto result = proccall_evaluator.evaluate(eval::ProcCall());

	ASSERT_EQ(get<Number>(result->get_data()), Number(3));

}

TEST(Proccall, three_argument_lambda) {

	using namespace shaka;
	auto lambda_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(Symbol("lambda")),
		DataNode::list(DataNode(Symbol("x")), DataNode(Symbol("y")), DataNode(Symbol("z"))),
		DataNode::list(DataNode(Symbol("+")), DataNode(Symbol("x")), DataNode(Symbol("y")),
			DataNode(Symbol("z")))
	));

	std::cout << *lambda_list << std::endl;	

	EnvPtr env = std::make_shared<shaka::Environment>(nullptr);
	env->set_value(Symbol("+"), make_node(NativeProcedure(stdproc::add, 1, true)));
	
	Evaluator lambda_evaluator(lambda_list->cdr(), env);
	
	auto proc = lambda_evaluator.evaluate(eval::Lambda());
	
	ASSERT_EQ(proc->is_procedure(), true);
	ASSERT_EQ(get<Procedure>(proc->get_data()).get_fixed_arity(), 3);
	ASSERT_EQ(get<Procedure>(proc->get_data()).is_variable_arity(), false);

	auto procedure_application_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(proc),
		DataNode(Number(1)),
		DataNode(Number(2)),
		DataNode(Number(3))
	));

	std::cout << *procedure_application_list << std::endl;

	Evaluator proccall_evaluator(procedure_application_list, env);

	auto result = proccall_evaluator.evaluate(eval::ProcCall());

	ASSERT_EQ(get<Number>(result->get_data()), Number(6));

}

TEST(Proccall, variable_arity_lambda) {
	
	using namespace shaka;


	auto lambda_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(Symbol("lambda")),
		DataNode(Symbol("x")),
		DataNode(Symbol("x"))
	));

	std::cout << *lambda_list << std::endl;	

	
	EnvPtr env = std::make_shared<shaka::Environment>(nullptr);
	
	Evaluator lambda_evaluator(lambda_list->cdr(), env);
	
	auto proc = lambda_evaluator.evaluate(eval::Lambda());
	
	ASSERT_EQ(proc->is_procedure(), true);
	ASSERT_EQ(get<Procedure>(proc->get_data()).get_fixed_arity(), 0);
	ASSERT_EQ(get<Procedure>(proc->get_data()).is_variable_arity(), true);

	auto proc_call_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(proc),
		DataNode(Number(1)),
		DataNode(Number(2)),
		DataNode(Number(3))
	));

	std::cout << *proc_call_list << std::endl;

	Evaluator proc_call_evaluator(proc_call_list, env);

	auto result = proc_call_evaluator.evaluate(eval::ProcCall());
	std::cout << *result << std::endl;
	ASSERT_EQ(Number(1), get<Number>(result->car()->get_data()));
}

TEST(Proccall, recursive_lambda_procedure) {
	using namespace shaka;

	auto lambda_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(Symbol("lambda")),
		DataNode::list(DataNode(Symbol("lst"))),
		DataNode::list(DataNode(Symbol("if")),
			DataNode::list(DataNode(Symbol("null?")),
				DataNode(Symbol("lst"))),
			DataNode(Number(0)),
			DataNode::list(
				DataNode(Symbol("+")),
				DataNode::list(DataNode(Symbol("car")),
					DataNode(Symbol("lst"))),
				DataNode::list(DataNode(Symbol("sum-list")),
					DataNode::list(DataNode(Symbol("cdr")),
						DataNode(Symbol("lst"))))))));
	
	std::cout << *lambda_list << std::endl;

	auto proc_call_list = std::make_shared<DataNode>(DataNode::list(
		DataNode(Symbol("sum-list")),
		DataNode::list(DataNode(Symbol("list")),
			DataNode(Number(1)), 
			DataNode(Number(2)), 
			DataNode(Number(3)))));

	std::cout << *proc_call_list << std::endl;

	EnvPtr env = std::make_shared<Environment>(nullptr);

	env->set_value(Symbol("+"), make_node(NativeProcedure(stdproc::add, 1, true)));
	env->set_value(Symbol("null?"), make_node(NativeProcedure(stdproc::nullp, 1, false)));
	env->set_value(Symbol("if"), make_node(PrimitiveProcedure(proc::if_expr, 3, false)));
	env->set_value(Symbol("car"), make_node(NativeProcedure(stdproc::car, 1, false)));
	env->set_value(Symbol("cdr"), make_node(NativeProcedure(stdproc::cdr, 1, false)));
	env->set_value(Symbol("list"), make_node(NativeProcedure(stdproc::list, 1, true)));
	Evaluator lambda_evaluator(lambda_list->cdr(), env);

	auto proc = lambda_evaluator.evaluate(eval::Lambda());

	ASSERT_EQ(proc->is_procedure(), true);
	ASSERT_EQ(get<Procedure>(proc->get_data()).get_fixed_arity(), 1);
	ASSERT_EQ(get<Procedure>(proc->get_data()).is_variable_arity(), false);

	env->set_value(Symbol("sum-list"), proc);

	Evaluator proc_call_evaluator(proc_call_list, env);

	auto result = proc_call_evaluator.evaluate(eval::ProcCall());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}