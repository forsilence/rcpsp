# include<iostream>
# include<fstream>
# include<string>
# include"include/rcpsp.h"
# include"include/text_handler.h"
RCPSP::priorityBG::priority_t RCPSP::priorityBG::max = 10;
int main()
{
    // RCPSP::test::infor_loader_test();
    // RCPSP::test::priorityBG_test();
    // RCPSP::test::ssgs_test();
    // RCPSP::test::ssgs_update_cut_set_test();
    // RCPSP::test::unordered_map_test();
    // RCPSP::test::topological_sort_test();
    // RCPSP::test::iterator_test();
    // RCPSP::test::condition_test();
    // RCPSP::test::evaluate_test();
    // RCPSP::test::quik_sort_test();
    // RCPSP::test::select_parents_test();
    RCPSP::test::ssgs_sort_test();
    return 0;
}