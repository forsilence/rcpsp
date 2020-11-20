# include<string>
# include<fstream>
# include<sstream>
# include<vector>
# include<algorithm>
# include<ctime>
# include<cstdlib>
# include<numeric>
# include"../include/rcpsp.h"
# include"../include/_M_processing_line.h"

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
    std::string file_type_dot_sm = ".sm";
    std::string file_type_dot_RCP = ".RCP";
    std::string::iterator dot_loc  = path.end();
    for(;dot_loc!=path.begin();dot_loc--)
        if(*dot_loc == file_type_dot_RCP[0])
            break;
    if(std::equal(dot_loc,path.end(),file_type_dot_sm.begin()))
        load_dot_sm_file(path);
    else if(std::equal(dot_loc,path.end(),file_type_dot_RCP.begin()))
        load_dot_RCP_file(path);
    else {
        std::cout << "not write load for this kind of file" << std::endl;
        exit(1);
    }
}
// load .sm file
// @arg std::string
void infor_loader::load_dot_sm_file(std::string path)
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

// load .RCP file
void infor_loader::load_dot_RCP_file(std::string path)
{
	// jobs-size limited-resources-size
    _M_th::text_handler::kv_t::iterator row_iterator = text_h.get_line_by_no(1);
    std::istringstream line(row_iterator->second);
    size_t jobs_size ;
    size_t limited_resources_size;
    line >> jobs_size >> limited_resources_size;
    std::cout << "limited resources size :" << limited_resources_size << std::endl;
    // limited-resource-size
    size_t limited_resource_size;
    row_iterator = text_h.get_line_by_no(2);
    std::istringstream line_2(row_iterator->second);
    job::resource_t res_no = 1;
    for(int i=0;i<limited_resources_size;i++) 
    {
        line_2 >> limited_resource_size;
        limited_resources.emplace(i+1,limited_resource_size);
    }
    // loading job information
    std::map<job::number_t,std::vector<job::number_t>> predecessors;
    for(int i=0;i<jobs_size;i++)
    {
        predecessors.clear();
        row_iterator = text_h.get_line_by_no(3+i);
        std::istringstream row_line(row_iterator->second);
        job::duration_t duration;// duration
        row_line >> duration ;
        job::resource_bulk_t required_res;
        required_res.clear();
        for(int j=0;j<limited_resources_size; j++)//required-resources
        {
            row_line >> limited_resource_size;
            required_res.emplace(j+1,limited_resource_size);
        }
        // successors
        // predecessors-store
        size_t successors_size ;
        row_line >> successors_size;
        std::vector<job::number_t> successors;
        job::number_t suc;
        while(row_line >> suc) 
        {
            successors.push_back(suc);
            predecessors[suc].push_back(i+1);
        }
        job new_job(i+1,successors);
        new_job.set_duration(duration);
        new_job.set_cur_mode(1);
        new_job.set_required_resources(required_res);
        jobs.emplace(i+1,new_job);
    }
    // set predecessors
    for(auto it:predecessors)
        jobs[it.first].set_predecessors(it.second);
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

// >>> job_scheduled_infor <<<
void job_scheduled_infor::print(std::ostream& print_out)
{
    print_out   << "job :" << no <<std::endl
                << "earliest start: " << earliest_start << std::endl
                << "earliest_finish: " << earliest_finish << std::endl
                << "latest start: " << latest_start << std::endl
                << "latest finish: " << latest_finish << std::endl;
}
// >>> priorityBG <<<
// copy constructor
priorityBG::priorityBG(const priorityBG& copy_source)
{
    _M_gene = copy_source._M_gene;
    result  = copy_source.result;
}
std::string priorityBG::to_string()
{
    std::string tmp;
    for(priority_t it:_M_gene)
        tmp += std::to_string(it);
    return tmp;
}
// >>> evaluate result<<<
void evaluate_result_t::print(std::ostream& print_out)
{
    print_out << "->last job" << std::endl;
    jsi.print(print_out);
    print_out << "->resource time line" << std::endl;
    for(auto it:resources_time_line)
    {
        print_out << "resource " << it.first << " time line " << std::endl;
        for(auto i:it.second)
            print_out << i.get_st() << "--" << i.get_et() << " " ;
        print_out << std::endl;
    }
    print_out << "->scheduled information for all jobs" << std::endl;
    for(auto it:scheduled_infor)
        it.second.print(print_out);
}
// >>> ssgs <<<
// @arg std::string
ssgs::ssgs(std::string project_file_path):ssgs()
{
    init(project_file_path);
}
ssgs::ssgs(std::string project_file_path,std::string log_file_path):ssgs(project_file_path)
{
    log.set_file(log_file_path);
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
// @arg std::vector<job::number_t>
// @return job_scheduled_infor
evaluate_result_t ssgs::evaluate( std::vector<job::number_t> topological_sort_result,
                                    priorityBG gene)
{
    infor_loader::no_job_t all_jobs_evaluate = get_all_jobs();
    job::resource_bulk_t limited_resources = get_resources();
    // initialization
    std::map<job::number_t,job_scheduled_infor> schedule_infor;
    job_scheduled_infor jsi;
    res_time_t res_use_scheduled_ls;
    for(job::resource_elem_t it:limited_resources)
    {
        time_line::time_bulk_t tb;
        res_use_scheduled_ls.emplace(it.first,tb);
    }
    // set the job 1(dummy job)
    jsi.set_es(0);
    jsi.set_ef(0);
    jsi.set_no(1);
    schedule_infor.emplace(1,jsi);
    infor_loader::no_job_t::iterator job_iterator;
    //  scheduling
    for(int i=1; i<topological_sort_result.size() ; i++)
    {
        jsi.set_no(topological_sort_result[i]);
        set_time(all_jobs_evaluate,schedule_infor,jsi,res_use_scheduled_ls);
        schedule_infor.emplace(topological_sort_result[i],jsi);
    }
    // return jsi;
    evaluate_result_t er(jsi,res_use_scheduled_ls,schedule_infor);
    return er;
}

// set time
// @arg infor_loader::no_job_t
// @arg std::map<job::number_t,job_scheduled_infor>
// @arg job_scheduled_infor&
// @arg res_time_t&
void ssgs::set_time(infor_loader::no_job_t all_jobs__,
                    std::map<job::number_t,job_scheduled_infor> schedule_infor,
                    job_scheduled_infor& jsi,
                    res_time_t& res_sl)
{
    // current scheduling job 
    infor_loader::no_job_t::iterator job_iterator = all_jobs__.find(jsi.get_no());
    // current scheduling job's predecessors
    std::vector<job::number_t> pres = job_iterator->second.get_predecessors();
    if(pres.empty()) { std::cout << "have no predecessors" << std::endl; exit(1);}
    // find the predecessor with max ef
    job::number_t last_ef_of_pre = pres[0];
    for(job::number_t it:pres)
    {
        std::map<job::number_t,job_scheduled_infor>::iterator jsi_iterator = schedule_infor.find(it);
        if(jsi_iterator!=schedule_infor.end())
            if(jsi_iterator->second.get_ef() 
                > schedule_infor.find(last_ef_of_pre)->second.get_ef())
                last_ef_of_pre  = jsi_iterator->first;
    }
    // set es and ef for cur job
    job_scheduled_infor::date_type es = schedule_infor.at(last_ef_of_pre).get_ef();
    job cur_job = all_jobs.at(jsi.get_no());
    for(res_time_t::iterator it=res_sl.begin();it!=res_sl.end();it++)
        if(!it->second.empty())
        {
            time_line::time_bulk_t::reverse_iterator tmp_date = it->second.rbegin();
            // find the earliest start time ,then start consider resource constrain 
            while(tmp_date != it->second.rend() && es < tmp_date->get_et())++tmp_date;
            // considering the resource constrain
            while(tmp_date != it->second.rbegin())
                if((es+cur_job.get_duration())>(--tmp_date)->get_st())
                    if (tmp_date->get_holding_resource_size() <= 
                        get_resources().at(it->first)-cur_job.get_required_resources().at(it->first) );
                    else es = tmp_date->get_et();
        }

    jsi.set_es(es);
    jsi.set_ef(es+cur_job.get_duration());
    // push time line to res_sl
    for(res_time_t::iterator it=res_sl.begin();it!=res_sl.end();it++)
        if(!it->second.empty())
        {
            time_line::time_bulk_t::reverse_iterator tmp_date = it->second.rbegin();
            time_line::time_bulk_t tmp_time_bulk ;
            while(tmp_date != it->second.rend() && es < tmp_date->get_et())++tmp_date;
            while(tmp_date != it->second.rbegin())
                if((es+cur_job.get_duration())>(--tmp_date)->get_st())
                    tmp_time_bulk.push_back(*tmp_date);
            time_line::time_bulk_t time_bulk_for_split;
            time_line::date_type walking_time = es;
            size_t cur_resource_size = cur_job.get_required_resources().at(it->first);
            for(int i=0;i<tmp_time_bulk.size();i++)
            {
                if(walking_time>tmp_time_bulk[i].get_st() )
                {
                    time_line behind_time = tmp_time_bulk[i];
                    time_line forward_time = tmp_time_bulk[i];
                    behind_time.set_et(walking_time);
                    forward_time.set_st(walking_time);
                    forward_time.set_holding_resource_size(
                                                        forward_time.get_holding_resource_size()+cur_resource_size);
                    time_bulk_for_split.push_back(behind_time);
                    time_bulk_for_split.push_back(forward_time);
                    walking_time = forward_time.get_et();
                }
                else if(walking_time == tmp_time_bulk[i].get_st())
                {
                    if(es+cur_job.get_duration() < tmp_time_bulk[i].get_et())
                    {
                        time_line behind_time = tmp_time_bulk[i];
                        time_line forward_time = tmp_time_bulk[i];
                        behind_time.set_et(walking_time);
                        forward_time.set_st(walking_time);
                        behind_time.set_holding_resource_size(
                                                            forward_time.get_holding_resource_size()+cur_resource_size);
                        walking_time = forward_time.get_et();
                        time_bulk_for_split.push_back(behind_time);
                        time_bulk_for_split.push_back(forward_time);
                    }
                    else
                    {
                        tmp_time_bulk[i].set_holding_resource_size(
                                            tmp_time_bulk[i].get_holding_resource_size()+cur_resource_size);
                        time_bulk_for_split.push_back(tmp_time_bulk[i]);
                    }
                }
                else
                {
                    if((es+cur_job.get_duration()) < tmp_time_bulk[i].get_et())
                    {
                        time_line behind_time = tmp_time_bulk[i];
                        time_line middl_time  = tmp_time_bulk[i];
                        time_line forward_time= tmp_time_bulk[i];
                        behind_time.set_st(walking_time);
                        behind_time.set_et(middl_time.get_st());
                        behind_time.set_holding_resource_size(cur_resource_size);
                        middl_time.set_et(es+cur_job.get_duration());
                        middl_time.set_holding_resource_size(middl_time.get_holding_resource_size()+cur_resource_size);
                        forward_time.set_st(es+cur_job.get_duration());
                        time_bulk_for_split.push_back(behind_time);
                        time_bulk_for_split.push_back(middl_time);
                        time_bulk_for_split.push_back(forward_time);
                        walking_time = forward_time.get_et();
                    }
                    else 
                    {
                        time_line behind_time = tmp_time_bulk[i];
                        time_line forward_time = tmp_time_bulk[i];
                        behind_time.set_st(walking_time);
                        behind_time.set_et(forward_time.get_st());
                        behind_time.set_holding_resource_size(cur_resource_size);
                        forward_time.set_holding_resource_size(forward_time.get_holding_resource_size()+cur_resource_size);
                        time_bulk_for_split.push_back(behind_time);
                        time_bulk_for_split.push_back(forward_time);
                        walking_time = forward_time.get_et();
                    }
                }
            }
            if(walking_time<es+cur_job.get_duration())
            {
                time_line last_time;
                last_time.set_st(walking_time);
                last_time.set_et(es+cur_job.get_duration());
                last_time.set_holding_resource_size(cur_resource_size);
                time_bulk_for_split.push_back(last_time);
            }
            time_line::time_bulk_t::iterator front = it->second.begin();
            time_line::time_bulk_t::iterator back  = it->second.begin();
            for(;front!=it->second.end() && back != it->second.end();)
            {
                if(front->get_et()<=es || back->get_st() < es+cur_job.get_duration() )
                {
                    if(front->get_et() <= es )
                        front++;
                    if(back->get_st() < es+cur_job.get_duration())
                        back++;
                }
                else break;
            }
            while(back!=it->second.end())
                time_bulk_for_split.push_back(*(back++));
            time_line::time_bulk_t::iterator tmp_split = time_bulk_for_split.begin();
            int time_bulk_walk_loc = 0;
            for(;front!=it->second.end() && time_bulk_walk_loc < time_bulk_for_split.size(); front++,time_bulk_walk_loc++)
                *front = time_bulk_for_split[time_bulk_walk_loc];
            while(time_bulk_walk_loc<time_bulk_for_split.size())
                it->second.push_back(time_bulk_for_split[time_bulk_walk_loc++]);
        }
        else
        {
            time_line first_time_line;
            first_time_line.set_st(jsi.get_es());
            first_time_line.set_et(jsi.get_ef());
            first_time_line.set_holding_resource_size(cur_job.get_required_resources().at(it->first));
            it->second.push_back(first_time_line);
        }
}
// objective function 
// @arg priorityBG
// @return time_line::date_type
time_line::date_type ssgs::objective_function(priorityBG gene)
{
    std::vector<job::number_t> topological_sort_result = topological_sort(gene);
    evaluate_result_t er = evaluate(topological_sort_result,gene);
    return er.get_jsi().get_ef();
}
void ssgs::ssgs_sort(   size_t pop_size=20,
                        int generation_size=20,
                        double crossover_rate = 0.1,
                        double mutate_rate = 0.1,
                        priorityBG::priority_t max=10
                        )
{
    print_line(0,generation_size);
    srand((unsigned)time(NULL));
    population_t pop = init_pop(pop_size,max);
    for(int i=0;i<generation_size;i++)
    {
        log.wirte_in(std::string("->>generation: ")+ std::to_string(i+1) +" <<-");
        for(int j=0;j<5;j++)
        {
            population_t crossover_parents = select_parents(pop);
            population_t children = crossover_2(crossover_parents,crossover_rate);
            for(population_t::value_type c:children)
            {
                mutate(c,mutate_rate,6);
                add_children_to_pop(pop,c);
            }
            pop_sort(pop);
            for(int i=0; i<pop.size()-pop_size ; i++)
            {
                population_t::iterator tmp_iter = pop.begin();
                pop.erase(tmp_iter);
            }
            // std::cout << "generation "<< i << "-> in loop:" << j << std::endl;
            // std::cout << "best result:" << best_result.get_result() << std::endl;
            log.wirte_in( "generation "+std::to_string(i+1)+"-> in loop:" + std::to_string(j));
            log.wirte_in("best result:"+std::to_string(best_result.get_result()));
        }
        print_line(i+1,generation_size);
    }
}

// init population(old version!)
// @arg size_t
// @return ssgs::population_t
ssgs::population_t ssgs::init_pop(  size_t pop_size,
                                    priorityBG::priority_t max)
{
    // init rand seed
    srand((unsigned)time(NULL));
    priorityBG::set_max(max);
    size_t gene_size = get_all_jobs().size();
    population_t pop;
    for(int i=0;i<pop_size;i++)
    {
        priorityBG new_gene;
        for(int j=0;j<gene_size;j++)
            new_gene.push_back(rand()%priorityBG::max);
        time_line::date_type tmp_result = objective_function(new_gene);
        new_gene.set_result(tmp_result);
        pop.push_back(new_gene);
    }
    pop_sort(pop);
    best_result = *pop.rbegin();
    log.wirte_in("best result:"+best_result.to_string());
    std::string tmp_str;
    for(auto it:pop)
        // std::cout << it.get_result() << " " ;
        tmp_str+=std::to_string(it.get_result())+" ";
    log.wirte_in(tmp_str);
    // std::cout << std::endl;
    return pop;
}
// convert objective value to adaptive value
// @arg std::vector<time_line::date_type>&
// @arg time_line::date_type
void ssgs::convert_objective_val_to_adaptive_val(
                                            std::vector<time_line::date_type>& objective_val_sl,
                                            time_line::date_type other_val=0.5)
{
    time_line::date_type max_val = objective_val_sl[0];
    time_line::date_type min_val = objective_val_sl[0];
    for(time_line::date_type it:objective_val_sl)
    {
        if(it>max_val) max_val = it;
        if(it<min_val) min_val = it;
    }
    // the core of function
    std::for_each(  objective_val_sl.begin(),
                    objective_val_sl.end(),
                    [max_val,min_val,other_val](time_line::date_type& a)
                            {a = (max_val-a+other_val)/(max_val-min_val+other_val);}
                    );
}

// pop sort
// @arg ssgs::population_t
void ssgs::pop_sort(population_t& pop)
{
    std::vector<time_line::date_type> pop_objective_val_ls;
    for(priorityBG it:pop)
        pop_objective_val_ls.push_back(it.get_result());
    convert_objective_val_to_adaptive_val(pop_objective_val_ls);
    quik_sort(pop,pop_objective_val_ls,0,pop.size()-1);
    // val list log
    std::string log_str;
    for(int i=0;i<pop_objective_val_ls.size();i++)
        log_str+=std::to_string(pop_objective_val_ls[i])+" ";
    log.wirte_in(log_str);
    // std::cout << std::endl;
    best_result = *pop.rbegin();
    log.wirte_in("best result:"+best_result.to_string());
}
// quik sort
int ssgs::quik_sort_partition(  population_t& pop,
                                std::vector<time_line::date_type>& val_ls,
                                int first,
                                int end)
{
    time_line::date_type flag = val_ls[end];
    int slower = first - 1;
    for(int i=first ; i<end ; i++)
        if(val_ls[i]<flag)
        {
            // swap 
            std::swap(val_ls[i],val_ls[++slower]);
            std::swap(pop[i],pop[slower]);
        }
    std::swap(val_ls[++slower],val_ls[end]);
    std::swap(pop[slower],pop[end]);
    return slower;
}
void ssgs::quik_sort(   population_t& pop,
                        std::vector<time_line::date_type>& val_ls,
                        int first,
                        int end)
{
    if(first<end)
    {
        int loc = quik_sort_partition(pop,val_ls,first,end);
        quik_sort(pop,val_ls,first,loc-1);
        quik_sort(pop,val_ls,loc+1,end);
    }
}
// select parents(this version too slow!better not use it)
// @arg ssgs::population
// @return ssgs::populatino
ssgs::population_t ssgs::select_parents(population_t pop)
{
    srand((unsigned)time(NULL));
    std::vector<time_line::date_type> vals;
    for(priorityBG it:pop)
        vals.push_back(it.get_result());
    convert_objective_val_to_adaptive_val(vals);
    // roulette wheel
    int p1 = roulette_wheel(vals);
    int p2 = p1;
    while(p2==p1) p2 = roulette_wheel(vals);
    population_t parents{pop[p1],pop[p2]};
    return parents;
}

// maybe not useful!
bool ssgs::gene_cmp(priorityBG a, priorityBG b)
{
    return objective_function(a) < objective_function(a);
}
// well-known roulette wheel
// @arg std::vector<time_line::date_time>
// @return int
int ssgs::roulette_wheel(std::vector<time_line::date_type> adaptive_val_ls)
{
    time_line::date_type sum = std::accumulate(adaptive_val_ls.begin(),adaptive_val_ls.end(),0);
    // time_line::date_type rand_val = ((double)rand())/32767.0;
    time_line::date_type rand_val = get_rand_val_0_to_1();
    time_line::date_type in_sum = 0;
    int result=0;
    for(;result<adaptive_val_ls.size();result++)
    {
        in_sum += adaptive_val_ls[result]/sum;
        if(rand_val<=in_sum) break;  
    }
    return result;
}
// add children to population
void ssgs::add_children_to_pop(population_t& pop,population_t::value_type c)
{
    pop.push_back(c);
    // pop_sort(pop);
    // population_t::iterator tmp = pop.begin();
    // pop.erase(tmp);
}
void ssgs::add_children_to_pop(population_t& pop,population_t c)
{
    for(population_t::value_type it:c)
        add_children_to_pop(pop,it);
}
// crossover
// @arg ssgs::population_t
// @arg double
// @return ssgs::population_t
ssgs::population_t ssgs::crossover(population_t parents ,double crossover_rate)
{
    srand((unsigned)time(NULL));
    population_t::value_type c1 = parents[0];
    population_t::value_type c2 = parents[1];
    for(int i=0; i<parents[0].size() ; i++)
        if(get_rand_val_0_to_1()<crossover_rate)
        {
            c1[i] = parents[1][i];
            c2[i] = parents[0][i];
        }
    c1.set_result(objective_function(c1));
    c2.set_result(objective_function(c2));
    return population_t{c1,c2};
}
// crossover_2
// @arg ssgs::population_t
// @arg double
// @return ssgs::population_t
ssgs::population_t ssgs::crossover_2(population_t parents , double crossover_rate)
{
    srand((unsigned)time(NULL));
    population_t::value_type c1 = parents[0];
    population_t::value_type c2 = parents[1];
    int gene_len = parents[0].size();
    for(int i=0; i<gene_len ; i++)
        if(get_rand_val_0_to_1()<crossover_rate)
        {
            c1[i] = parents[1][i+rand()%(gene_len-i)];
            c2[i] = parents[0][i+rand()%(gene_len-i)];
        }
    c1.set_result(objective_function(c1));
    c2.set_result(objective_function(c2));
    return population_t{c1,c2};
}
// local search-based mutate
// @arg priorityBG&
// @arg double
// @arg int
void ssgs::mutate(  priorityBG& ind,
                    double mutate_rate,
                    int neighborhood_width )
{
    // get center gene
    population_t neighborhood(neighborhood_width,ind);
    int center_gene_loc = rand()%ind.size();
    for(int i=0 ; i<neighborhood_width ; i++)
    {
        int neighborhood_loc = -1;
        for(int j=0;j<neighborhood[i].size() && neighborhood_loc<0;j++)
            if(get_rand_val_0_to_1()<mutate_rate)
                neighborhood_loc = j;
        if(neighborhood_loc>0)
        {
        neighborhood[i][center_gene_loc] = ind[neighborhood_loc];
        neighborhood[i][neighborhood_loc] = ind[center_gene_loc];
        }
        neighborhood[i].set_result( objective_function(neighborhood[i]) );
    }
    for(priorityBG it:neighborhood)
        if(it.get_result()<ind.get_result())
            ind = it;
}
// get rand val (0 to 1)
// @return double
double ssgs::get_rand_val_0_to_1()
{
    return ((double)rand())/32767.0;
}

// log
void ssgs::set_log_file(std::string log_file)
{
    log.set_file(log_file);
}
void ssgs::set_log_buffer_size(size_t buffer_size)
{
    log.set_buffer_size(buffer_size);
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
    for(auto it:il.get_all_jobs_map())
        it.second.print(std::cout);
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

void test::iterator_test()
{
    std::cout << ">>> iterator test <<<" << std::endl;
    std::vector<int> vc(10);
    std::cout << "->size " << vc.size() << std::endl;
    for(int i=0;i<10 ; i++)
        vc[i] = i;
    for(int i:vc) 
        std::cout << std::cout.width(2) ;
    std::cout << std::endl;
    auto tmp = vc.begin();
    if(--tmp == vc.end())
        std::cout << "--vc.begin() == vc.end()" << std::endl;
    else std::cout << "--vc.begin() != vc.end()" << std::endl;
    std::cout << ">>> end of iterator test <<<" << std::endl;
}

void test::condition_test()
{
    std::cout << ">>> condition test <<<" << std::endl;
    int count = 0;
    while(true)
        if(count<10)
            std::cout << std::cout.width(2) << count++;
        else break;
    std::cout << std::endl << ">>> end of condition test <<<" << std::endl;
}

void test::evaluate_test()
{
    std::cout << ">>> evaluate test <<<" << std::endl;
    priorityBG gene;
    srand((unsigned)time(NULL));
    // ssgs s("j301_4.sm");
    ssgs s("j60.sm/j601_1.sm");
    // ssgs s("j120.sm/j1201_1.sm");
    for(int i=0;i<s.get_all_jobs().size();i++)
        gene.push_back(rand()%10);
    gene[gene.size()-1]= -1;
    evaluate_result_t jsi = s.evaluate(s.topological_sort(gene),gene);
    jsi.print(std::cout);
    std::cout << ">>> end of evaluate test <<<" << std::endl;
}

void test::quik_sort_test()
{
    std::cout << ">>> quik-sort test <<<" << std::endl;
    ssgs s("j301_4.sm");
    s.init_pop(10,10);
    std::cout << ">>> end of quik-sort test <<<" << std::endl;
}

void test::select_parents_test()
{
    std::cout << ">>> select_parents test <<<" << std::endl;
    ssgs s("j301_4.sm");
    std::vector<time_line::date_type> vals;
    ssgs::population_t pop = s.init_pop(10,10);
    for(auto it:s.select_parents(pop))
    {
        for(int i=0;i<it.size();i++)
            std::cout << it[i] << " ";
        std::cout << std::endl;
    }
    std::cout << ">>> end of select_parents test <<<" << std::endl;
}
void test::ssgs_sort_test(std::string path)
{
    std::cout << ">>> ssgs_sort test <<<" << std::endl;
    priorityBG::max = 10;
    ssgs s(path,path+"_log_file"+"2");
    size_t pop_size = 20;
    int generation_size = 100;
    double crossover_rate = 0.2;
    double mutate_rate = 0.1;
    s.write_in("pop-size :" + std::to_string(pop_size) +
                " ,population-size :" + std::to_string(generation_size) + 
                " ,crossover-rate :"+ std::to_string(crossover_rate) + 
                " ,mutate-rate :"+ std::to_string(mutate_rate));
    s.ssgs_sort(pop_size,generation_size,crossover_rate,mutate_rate);
    std::cout << ">>> end of ssgs_sort test <<<" << std::endl;
}
void test::load_dot_RCP_file_test()
{
    std::cout << ">>> load .RCP file test <<<" << std::endl;
    ssgs s("src/J301_1.RCP");
    std::ofstream ofile;
    ofile.open("load_dot_RCP_file_test",std::ofstream::ate);
    for(auto it:s.get_all_jobs())
        it.second.print(ofile);
    for(auto it:s.get_resources())
        ofile << "resource " << it.first << " size " << it.second << std::endl;
    ofile.close();
    std::cout << ">>> end of test <<<" << std::endl;
}
} // namespace RCPSP
