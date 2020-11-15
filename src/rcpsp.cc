# include<string>
# include<fstream>
# include<sstream>
# include<vector>
# include<algorithm>
# include<ctime>
# include<cstdlib>
# include"../include/rcpsp.h"

namespace RCPSP
{
// >>> job <<<
// @arg std::ostream
// @return std::ostream
std::ostream& job::print(std::ostream& out)
{
    out << "job_nb:" << job_nb << std::endl;
    out << "successors:" ;
    for(number_t it:successors)
        out << it << ' ' ;
    out << std::endl;
    out << "predecussors:";
    for(number_t it:predecessors)
        out << it << " ";
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
// @arg std::string
infor_loader::infor_loader(std::string path):
                                            text_h(path),
                                            jobs(),
                                            limited_resources()
{
    _M_th::text_handler::row_no_bulk_t rows_of_jobs = text_h.find_word_row_no("jobs");
    _M_th::text_handler::row_no_t first_row_no_of_jobs = rows_of_jobs[0];
    _M_th::text_handler::kv_t::iterator line_of_word_jobs_iterator = text_h.get_line_by_no(first_row_no_of_jobs);
    _M_th::line_handler lh(line_of_word_jobs_iterator->second);
    job::size_type jobs_size = std::atoi(lh.last()->c_str());

    _M_th::text_handler::row_no_bulk_t rows_of_word_jobnr = text_h.find_word_row_no("jobnr.");
    _M_th::text_handler::kv_t::iterator iterator_start_by_word_jobnr = text_h.get_line_by_no(rows_of_word_jobnr[0]);
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

    std::map<job::number_t,std::vector<job::number_t>> store_predecessors;
    if(iterator_start_by_word_jobnr!=text_h.end_of_lines())
    for(++iterator_start_by_word_jobnr;
        job_no!=jobs_size;
        iterator_start_by_word_jobnr++)
    {
        // job_no
        // successors
        // predecessors(load)
        std::istringstream line(iterator_start_by_word_jobnr->second);
        line >> job_no >> mode_size >> su_size ;
        tmp_sus.clear();
        while(line >> su )
        {
            tmp_sus.push_back(su);
            // add predecessors
            std::map<   job::number_t,
                        std::vector<job::number_t>>::iterator
                store_pred_iter = store_predecessors.find(su);
            if(store_pred_iter==store_predecessors.end())
            {
                std::vector<job::number_t> preds{job_no};
                store_predecessors.emplace(su,preds);
            }
            else
                if( std::find(
                        store_pred_iter->second.begin(),
                        store_pred_iter->second.end(),
                        job_no)
                    == 
                    store_pred_iter->second.end())
                    store_pred_iter->second.push_back(job_no);
        }
        job new_job(job_no,tmp_sus);
        jobs.insert(no_job_elem_t(job_no,new_job));
    }
    else std::cout << "error!" << std::endl;
    std::cout << "predecessors size " << store_predecessors.size() << std::endl;

    iterator_start_by_word_jobnr = text_h.get_line_by_no(rows_of_word_jobnr[1]);
    job_no=0;
    if(iterator_start_by_word_jobnr!=text_h.end_of_lines())
    for(++(++iterator_start_by_word_jobnr);
        job_no!=jobs_size;
        iterator_start_by_word_jobnr++)
    {
        // mode
        // duration
        // required_resources
        // predecessors(add)
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
        if(job_no!=1)// job 1 not add to store_predecessors
            job_iterator->second.set_predecessors(store_predecessors.at(job_no));
    }
    else std::cout << "error2!" << std::endl;

    // init limited resources !
    rows_of_word_jobnr = text_h.find_word_row_no("RESOURCEAVAILABILITIES:");
    iterator_start_by_word_jobnr = text_h.get_line_by_no(rows_of_word_jobnr[0]);
    ++(++iterator_start_by_word_jobnr);
    std::istringstream line(iterator_start_by_word_jobnr->second);
    rec = 1;
    while(line >> su_size)
        limited_resources.emplace(rec++,su_size);
}

// @arg job::number_t
// @return infor_loader::no_job_t::iterator
infor_loader::no_job_t::iterator infor_loader::find_job_by_no(job::number_t no)
{
    return jobs.find(no);
}
// @return infor_loader::no_job_t::iterator
infor_loader::no_job_t::iterator infor_loader::end_of_jobs()
{
    return jobs.end();
}

// @arg job::resource_bulk_t
// @return void
void infor_loader::update_resources(job::resource_bulk_t rss)
{
    limited_resources.clear();//not need
    limited_resources = rss;
}
// >>> ssgs <<<
// @arg std::string
ssgs::ssgs(std::string project_file_path):ssgs()
{
    init(project_file_path);
}
// @arg std::string
void ssgs::init(std::string project_file_path)
{
    // init members: load all jobs and limited resources  
    // load inforation from file
    infor_loader il(project_file_path);
    all_jobs = il.get_all_jobs_map();
    resources_set = il.get_resources();
}

// @arg ssgs::population_t
// @return ssgs::topological_sort_rt
std::vector<job::number_t> ssgs::topological_sort(priorityBG gene)
{
    // init
    infor_loader::no_job_t free_nodes = get_all_jobs();
    infor_loader::no_job_t sorted_nodes;
    std::vector<job::number_t> PS;// the real order
    // push the job 1 (dummy node) to sorted_nodes and delete job 1 from free_nodes
    infor_loader::no_job_t::iterator job_iterator = free_nodes.begin();
    sorted_nodes.emplace(job_iterator->first, job_iterator->second);
    PS.push_back(job_iterator->first);
    free_nodes.erase(job_iterator);
    // init priority quene
    infor_loader::no_job_t priority_quene;
    for(job::number_t it:sorted_nodes.begin()->second.get_successors())
    {
        job_iterator = free_nodes.find(it);
        priority_quene.emplace(job_iterator->first,job_iterator->second);
        free_nodes.erase(job_iterator);
    }
    // init cur set 
    cut_set_t cut_set;
    update_cut_set(sorted_nodes,cut_set);
    // core of topological sort
    int count = 1;
    while(sorted_nodes.size() != gene.size())
    {
        job::number_t max_pr_job = max_priority(priority_quene,gene);
        infor_loader::no_job_t::iterator job_iterator_in_loop = priority_quene.find(max_pr_job);
        if(job_iterator_in_loop != priority_quene.end())
        {
            sorted_nodes.emplace(max_pr_job,job_iterator_in_loop->second);
            PS.push_back(max_pr_job);
            priority_quene.erase(job_iterator_in_loop);
        }
        update_cut_set(sorted_nodes,cut_set);
        size_t cut_set_size = cut_set.size();
        update_priority_nodes(sorted_nodes,cut_set,free_nodes,priority_quene);
    }
    return PS;
}
// @arg infor_loader::no_job_t
// @arg job
// @return bool
bool ssgs::eligible(    infor_loader::no_job_t sorted_nodes,
                        job j)
{
    for(job::number_t pre:j.get_predecessors())
        if(sorted_nodes.find(pre) == sorted_nodes.end())
            return false;
    return true;
}
// @arg infor_loader::no_job_t
// @arg infor_loader::no_job_t
// @arg job::number_t
// @return bool
bool ssgs::eligible(    infor_loader::no_job_t sorted_nodes,
                        infor_loader::no_job_t free_nodes,
                        job::number_t no)
{
    job checking_job = free_nodes.at(no);
    return eligible(sorted_nodes,checking_job);
}

// uodate cut set
// @arg infor_loader::no_job_t 
// @arg ssgs::cut_set_t
// @return ssgs::cut_set_t
ssgs::cut_set_t& ssgs::update_cut_set(   infor_loader::no_job_t sorted_nodes,
                                        cut_set_t& cut_set)
{
    cut_set.clear();// necessary?
    // start at the first nodes(dummy job)
    update_cut_set_partial( sorted_nodes,
                            cut_set,
                            sorted_nodes.begin()->first);
    return cut_set;
}
// update_cut_set_partial (recursive function)
// @arg infor_loader::no_job_t
// @arg ssgs::cut_set_t
// @arg job::number_t
void ssgs::update_cut_set_partial( infor_loader::no_job_t sorted_nodes,
                                    cut_set_t& cut_set,
                                    job::number_t cur_job)
{
    if(sorted_nodes.empty()) return;
    infor_loader::no_job_t::iterator job_iterator = sorted_nodes.find(cur_job);
    if(job_iterator==sorted_nodes.end()) return;
    std::vector<job::number_t> exist_sccessors;
    std::vector<job::number_t> successors = job_iterator->second.get_successors();
    // check if has successors in sorted nodes ,if have then recursive loop
    for(job::number_t it:successors)
    {
        infor_loader::no_job_t::iterator job_iterator_in_loop = sorted_nodes.find(it);
        if(job_iterator_in_loop!=sorted_nodes.end())
        {
            update_cut_set_partial( sorted_nodes,
                                    cut_set,
                                    job_iterator_in_loop->first);
            exist_sccessors.push_back(job_iterator_in_loop->first);
        }
    }
    if(exist_sccessors.empty())// at cut set
        for(job::number_t it:successors)
            cut_set.emplace(job_iterator->first,it);
}

// update_priority_nodes
// @arg infor_loader::no_job_t 
// @arg cut_set_t
// @arg infor_loader::no_job_t&
// @arg infor_loader::no_job_t&
void ssgs::update_priority_nodes(   infor_loader::no_job_t sorted_nodes,
                                    cut_set_t cut_set,
                                    infor_loader::no_job_t& free_nodes,
                                    infor_loader::no_job_t& priority_quene)
{
    for(cut_set_elem_t it:cut_set)
    {
        infor_loader::no_job_t::iterator job_iterator = free_nodes.find(it.second);
        if(job_iterator!=free_nodes.end())
        if(eligible(sorted_nodes,job_iterator->second))
        {
            priority_quene.emplace(job_iterator->first,job_iterator->second);
            free_nodes.erase(job_iterator);
        }
    }
}

job::number_t ssgs::max_priority(   infor_loader::no_job_t pr_quene,
                                    priorityBG gene) const 
{
    job::number_t max_job = pr_quene.begin()->first;
    for(infor_loader::no_job_elem_t it:pr_quene)
        if(gene[it.first] > gene[max_job])
            max_job = it.first;
    return max_job;
}
// evaluate
// @arg infor_loader::no_job_t
// @return job_scheduled_infor
job_scheduled_infor ssgs::evaluate( infor_loader::no_job_t topological_sort_result,
                                    priorityBG gene)
{
    // initialization
    std::map<job::number_t,job_scheduled_infor> schedule_infor;
    job_scheduled_infor jsi;
    jsi.set_es(0);
    jsi.set_ef(0);
    return jsi;
}
// parallel schedule generation scheme
void psgs()
{
}
// >> test zone <<
void test::infor_loader_test()
{
    std::cout << ">>> infor_loader test <<<" << std::endl;
    infor_loader il("j301_4.sm");
    std::cout << "size of jobs:" << il.size_of_jobs() << std::endl;
    infor_loader::no_job_t::iterator tmp = il.find_job_by_no(1);
    std::ofstream out;
    out.open("jobs",std::ofstream::out);
    for(;tmp!=il.end_of_jobs();tmp++)
        tmp->second.print(out) << std::endl;
    for(job::resource_elem_t i:il.get_resources())
        std::cout << "resource " << i.first << " size :" << i.second << std::endl;
    std::cout << ">>> end of infor_loader test <<<" << std::endl;
}
void test::priorityBG_test()
{
    std::cout << ">>> priorityBG test <<<" << std::endl;
    // init a priorityBG
    priorityBG pbg;
    for(int i=0 ; i<10 ; i++){
        pbg.push_back(i);
    }
    std::cout << "->init val" << std::endl;
    for(int i=0 ; i<pbg.size() ; i++)
        std::cout << pbg[i] << " " ;
    std::cout << std::endl;
    std::cout << "->change location 1 node val to 100" << std::endl;
    std::cout << "->use operator[] = val " << std::endl;
    pbg[1] = 100;
    std::cout << "->after operate" << std::endl;
    for(int i=0 ; i<pbg.size() ; i++)
        std::cout << pbg[i] << " " ;
    std::cout << std::endl;
    std::cout << ">>> end of priorityBG test <<<" << std::endl;
} 

// ssgs test
void test::ssgs_test()
{
    std::cout << ">>> ssgs-test <<<" << std::endl;
    ssgs s("j301_4.sm");
    std::cout << "->all jobs" << std::endl;
    std::ofstream ofile;
    ofile.open("ssgs_test",std::ofstream::out);
    for(auto it:s.get_all_jobs())
        it.second.print(ofile) << std::endl;
    std::cout << std::endl << "->resources" << std::endl;
    for(auto it:s.get_resources())
        std::cout << "->resource " << it.first << " size :" << it.second << std::endl;
    std::cout << ">>> end of ssgs test <<<" << std::endl;
}

// ssgs_update_cut_set_test
void test::ssgs_update_cut_set_test()
{
    std::cout << ">>> ssgs update cut set test <<<" << std::endl;
    // init
    ssgs s("j301_4.sm");
    infor_loader::no_job_t free_nodes = s.get_all_jobs();
    infor_loader::no_job_t sorted_nodes;
    // push the job 1 (dummy node) to sorted_nodes and delete job 1 from free_nodes
    infor_loader::no_job_t::iterator job_iterator = free_nodes.begin();
    sorted_nodes.emplace(job_iterator->first, job_iterator->second);
    free_nodes.erase(job_iterator);
    ssgs::cut_set_t cut_set;
    s.update_cut_set(sorted_nodes,cut_set);
    job_iterator = free_nodes.begin();
    sorted_nodes.insert(infor_loader::no_job_elem_t( job_iterator->first,job_iterator->second ));
    // init cur set 
    s.update_cut_set(sorted_nodes,cut_set);
    std::cout << "->after 1 job add, the cut set will be" << std::endl;
    for(auto it:cut_set)
        std::cout << it.first << " " << it.second << std::endl;
    std::cout << ">>> end of ssgs update cut set test <<<" << std::endl;
}

void test::unordered_map_test()
{
    std::cout << ">>> unordered_map test <<<" << std::endl;
    std::unordered_map<int,int> t;
    t.emplace(1,2);
    t.emplace(1,3);
    t.emplace(2,3);
    for(auto it:t)
        std::cout << "first " << it.first << " second " << it.second << std::endl;
    std::cout << ">>> end of unordered_map test <<<" << std::endl;
}

void test::topological_sort_test()
{
    std::cout << ">>> topological_sort test <<<" << std::endl;
    priorityBG gene;
    srand((unsigned)time(NULL));
    ssgs s("j301_4.sm");
    for(int i=0;i<s.get_all_jobs().size();i++)
        gene.push_back(rand()%10);
    gene[gene.size()-1]= -1;
    std::cout << "->jobs initial order" << std::endl;
    for(auto it:s.get_all_jobs())
        std::cout << std::cout.width(3) << it.first ;
    std::cout << std::endl;
    std::cout << "->order after topological sort" << std::endl;
    for(auto it:s.topological_sort(gene))
        std::cout << std::cout.width(3) << it ;
    std::cout << std::endl;
    std::cout << ">>> end of topological_sort test <<<" << std::endl;
}
} // namespace RCPSP