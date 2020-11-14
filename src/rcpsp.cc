# include<string>
# include<sstream>
# include<vector>
# include"../include/rcpsp.h"

// <<- - job - ->>
std::ostream& RCPSP::job::print(std::ostream& out)
{
    out << "job_nb:" << job_nb << std::endl;
    out << "successors:" ;
    for(number_t it:successors)
        out << it << ' ' ;
    out << std::endl;
    out << "modes:";
    for(mode_t it:modes)
        out << it;
    out << std::endl;
    out << "cur_mode:" << cur_mode << std::endl;
    out << "duration:" << duration << std::endl;
    out << "required sources:" << std::endl;
    for( resource_elem_t it: required_resources)
        out << "resource " << it.first << " required size :" << it.second << std::endl;
    return out;
}
// information loader
RCPSP::infor_loader::infor_loader(std::string path):text_h(path),jobs()
{
    text_handler::row_no_bulk_t rows_of_jobs = text_h.find_word_row_no("jobs");
    text_handler::row_no_t first_row_no_of_jobs = rows_of_jobs[0];
    text_handler::kv_t::iterator line_of_word_jobs_iterator = text_h.get_line_by_no(first_row_no_of_jobs);
    line_handler lh(line_of_word_jobs_iterator->second);
    job::size_type jobs_size = std::atoi(lh.last()->c_str());

    text_handler::row_no_bulk_t rows_of_word_jobnr = text_h.find_word_row_no("jobnr.");
    text_handler::kv_t::iterator iterator_start_by_word_jobnr = text_h.get_line_by_no(rows_of_word_jobnr[0]);
    job::number_t job_no=0;//
    job::size_type mode_size;
    job::size_type su_size;
    job::number_t su;//successors
    job::mode_t mode;//mode
    std::vector<job::mode_t> modes;//modes
    job::resource_bulk_t::key_type rkey;
    job::resource_t rec;
    std::vector<job::number_t> tmp_sus;
    job::duration_t duration;//duration
    job::resource_bulk_t res;

    if(iterator_start_by_word_jobnr!=text_h.end_of_lines())
    for(++iterator_start_by_word_jobnr;job_no!=jobs_size;iterator_start_by_word_jobnr++)
    {
        // job_no
        // su
        std::istringstream line(iterator_start_by_word_jobnr->second);
        line >> job_no >> mode_size >> su_size ;
        tmp_sus.clear();
        while(line >> su )
            tmp_sus.push_back(su);
        job new_job(job_no,tmp_sus);
        jobs.insert(no_job_elem_t(job_no,new_job));
    }
    else std::cout << "error!" << std::endl;

    iterator_start_by_word_jobnr = text_h.get_line_by_no(rows_of_word_jobnr[1]);
    job_no=0;
    if(iterator_start_by_word_jobnr!=text_h.end_of_lines())
    for(++iterator_start_by_word_jobnr;job_no!=jobs_size;iterator_start_by_word_jobnr++)
    {
        std::istringstream line(iterator_start_by_word_jobnr->second);
        line >> job_no >> mode >> duration ;
        rec = 1;
        res.clear();
        while(line >> su_size)
            res.insert(job::resource_elem_t(rec++,su_size));
        no_job_t::iterator job_iterator = find_job_by_no(job_no);
        job_iterator->second.set_cur_mode(mode);
        job_iterator->second.set_duration(duration);
        job_iterator->second.set_required_resources(res);
    }
    else std::cout << "error2!" << std::endl;
}

RCPSP::infor_loader::no_job_t::iterator RCPSP::infor_loader::find_job_by_no(job::number_t no)
{
    return jobs.find(no);
}
RCPSP::infor_loader::no_job_t::iterator RCPSP::infor_loader::end_of_jobs()
{
    return jobs.end();
}

// serial schedule generation scheme 
void RCPSP::ssgs::scheduling(std::string project_file_path)
{
    infor_loader il(project_file_path);
    infor_loader::no_job_t init_group_map = il.get_all_jobs_map();
    // init scheduled set :put 1 job(dummy job) to scheduled set 
    infor_loader::no_job_t scheduled_set;
    infor_loader::no_job_t::iterator no_job_t_iter = init_group_map.begin();
    scheduled_set.emplace(no_job_t_iter->first,no_job_t_iter->second);
    init_group_map.erase(no_job_t_iter);
    // init decision set: put successors of 1 job to decision set
    update_decision_set();
}

void RCPSP::ssgs::update_decision_set()
{
    for(infor_loader::no_job_elem_t it:scheduled_set)
    {
        for(job::number_t i:it.second.get_successors())
        {
            infor_loader::no_job_t::iterator no_job_t_iter = init_group_map.find(i);
            if(no_job_t_iter!=init_group_map.end())
            {
                decision_set.emplace(no_job_t_iter->first,no_job_t_iter->second);
                init_group_map.erase(no_job_t_iter);
            }
        }
    }
}

// parallel schedule generation scheme
void RCPSP::psgs()
{
}