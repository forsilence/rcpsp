# include<iostream>
# include<fstream>
# include<string>
# include"include/rcpsp.h"
# include"include/text_handler.h"

int main()
{
    RCPSP::test::infor_loader_test();
    RCPSP::test::priorityBG_test();
    RCPSP::test::ssgs_test();
    RCPSP::test::ssgs_update_cut_set_test();
    // RCPSP::test::unordered_map_test();
    RCPSP::test::topological_sort_test();
    return 0;
}