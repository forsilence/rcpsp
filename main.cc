# include<iostream>
# include<fstream>
# include<string>
# include"include/rcpsp.h"
# include"include/text_handler.h"

int main()
{
    RCPSP::infor_loader il("j301_4.sm");
    std::cout << "size of jobs:" << il.size_of_jobs() << std::endl;
    RCPSP::infor_loader::no_job_t::iterator tmp = il.find_job_by_no(1);
    std::ofstream out;
    out.open("jobs",std::ofstream::out);
    for(;tmp!=il.end_of_jobs();tmp++)
        tmp->second.print(out) << std::endl;
    // for(RCPSP::job::resource_elem_t i:il.get_resources())
    //     std::cout << "resource " << i.first << " size :" << i.second << std::endl;
    return 0;
}