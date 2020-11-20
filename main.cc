# include<iostream>
# include<fstream>
# include<string>
# include"include/rcpsp.h"
# include"include/text_handler.h"
RCPSP::priorityBG::priority_t RCPSP::priorityBG::max = 10;
int main(int argc,char *argv[])
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
    // RCPSP::test::ssgs_sort_test("j30.sm/j303_10.sm");
    // _M_th::test::text_writer_test();
    // args: file pop-size generation-size crossover_rate mutate_rate
    if(argc>1)
    {
        RCPSP::ssgs s(argv[1]);
        size_t pop_size = 20;
        int generation_size = 40;
        double crossover_rate = 0.1;
        double mutate_rate = 0.1;
        if(argc>2)
            pop_size = std::atoi(argv[2]);
        if(argc>3)
            generation_size = std::atoi(argv[3]);
        if(argc>4)
            crossover_rate = std::atof(argv[4]);
        if(argc>5)
            mutate_rate = std::atof(argv[5]);
        s.set_log_file(std::string(argv[1])+"_log_file");
        s.write_in( "pop-size :" + std::to_string(pop_size) +
                    " ,population-size :" + std::to_string(generation_size) + 
                    " ,crossover-rate :"+ std::to_string(crossover_rate) + 
                    " ,mutate-rate :"+ std::to_string(mutate_rate));
        s.ssgs_sort(pop_size,generation_size,crossover_rate,mutate_rate,10);
    }
    else
        std::cerr << "format command :ssgsSort file pop-size(20) generation-size(40) crossover-rate(0.1) mutate-rate(0.1)" << std::endl;
    return 0;
}