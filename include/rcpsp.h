# if! defined(_RCPSP_H)
# define _RCPSP_H
# include<vector>
# include<cstdlib>
# include<map>
# include<unordered_map>
# include"text_handler.h"

namespace RCPSP
{
    
// resource constrained project scheduling problem
class job
{
    public:
        typedef unsigned int number_t;
        typedef size_t size_type;
        typedef unsigned int duration_t;
        typedef unsigned int mode_t;
        typedef unsigned int resource_t;
        typedef std::map<   resource_t ,
                            size_type> resource_bulk_t;
        typedef std::pair<  resource_t ,
                            size_type> resource_elem_t;

        job()=default;
        job(number_t J_nb,
            std::vector<number_t> Ss
            ):  job_nb(J_nb),
                successors(Ss),
                required_resources(){}
        job(number_t J_nb,
            size_type Modes_size,
            size_type Ss_size,
            std::vector<number_t> Ss
            ):  job(J_nb,Ss){}
        job(number_t J_nb,
            std::vector<mode_t> Successors,
            std::vector<mode_t> Modes,
            mode_t Cur_mode,
            duration_t Duration,
            resource_bulk_t Required_resources
            ):  job_nb(J_nb),
                successors(Successors),
                modes(Modes),
                cur_mode(Cur_mode),
                duration(Duration),
                required_resources(Required_resources){}

        number_t get_number() const { return job_nb; }
        void set_number(number_t j_n) { job_nb=j_n; }
        size_type get_mode_size() const { return modes.size() ; }
        std::vector<mode_t> get_modes() const ;
        void set_modes(std::vector<mode_t> ms) { modes = ms; }
        mode_t get_cur_mode() const { return cur_mode; }
        void set_cur_mode(mode_t cm) { cur_mode = cm; }
        size_type get_successors_size() const { return successors.size(); }
        std::vector<number_t> get_successors() const { return successors; }
        void set_successors(std::vector<number_t> sus) { successors=sus; }
        duration_t get_duration() const { return duration; }
        void set_duration(duration_t dura) { duration = dura; }
        resource_bulk_t get_required_resources() const { return required_resources; }
        void set_required_resources(resource_bulk_t rrs) { required_resources = rrs; }
        void set_predecessors(std::vector<number_t> pdcs) { predecessors = pdcs; }
        std::vector<number_t> get_predecessors() const { return predecessors; }

        std::ostream& print(std::ostream& );

    private:
        number_t job_nb;// 号码
        std::vector<number_t> successors;
        std::vector<number_t> predecessors;
        std::vector<mode_t> modes;
        mode_t cur_mode;
        duration_t duration;
        resource_bulk_t required_resources;
};

// >>>information loader<<<
class infor_loader
{
    public:
        typedef std::map<job::number_t,job> no_job_t;
        typedef std::pair<job::number_t,job> no_job_elem_t;

        infor_loader() = default;
        infor_loader(std::string path);

        void load_dot_sm_file(std::string );
        void load_dot_RCP_file(std::string );
        size_t size_of_jobs() const { return jobs.size(); }
        no_job_t::iterator find_job_by_no(job::number_t);
        no_job_t::iterator begin_of_jobs();
        no_job_t::iterator end_of_jobs();
        no_job_t get_all_jobs_map() const { return jobs; }
        job::resource_bulk_t get_resources() const { return limited_resources; }
        void update_resources(job::resource_bulk_t rss);
    private:
        _M_th::text_handler text_h;
        std::map<job::number_t,job> jobs;
        job::resource_bulk_t limited_resources;
};

// >>>job_scheduled_infor<<<
class job_scheduled_infor
{
    public:
        typedef double date_type;

        void set_no(job::number_t no_) { no = no_; }
        void set_es(date_type es) { earliest_start = es; }
        void set_ef(date_type ef) { earliest_finish = ef; }
        void set_ls(date_type ls) { latest_start = ls; }
        void set_lf(date_type lf) { latest_finish = lf; }
        job::number_t get_no() const { return no; }
        date_type get_es() const { return earliest_start; }
        date_type get_ef() const { return earliest_finish; }
        date_type get_ls() const { return latest_start; }
        date_type get_lf() const { return latest_finish; }
        void print(std::ostream&);
    private:
        date_type earliest_start;// 最早开始时间
        date_type earliest_finish;// 最早结束时间
        date_type latest_start;// 最晚开始时间
        date_type latest_finish;// 最晚结束时间
        job::number_t no;
};
// >>>time_line<<<
class time_line
{
    public:
        typedef std::vector<time_line> time_bulk_t;
        typedef job_scheduled_infor::date_type date_type;
        time_line() = default;
        time_line(  date_type st,
                    date_type et
                    ):  start_time(st),
                        end_time(et),
                        holding_resources_size(0){}
        void set_st(date_type st_) { start_time = st_; }
        void set_et(date_type et_) { end_time = et_; }
        date_type get_st() const { return start_time; }
        date_type get_et() const { return end_time; }
        void set_holding_resource_size(size_t s) { holding_resources_size = s; }
        size_t get_holding_resource_size() const { return holding_resources_size; }
    private:
        date_type start_time;
        date_type end_time;
        size_t holding_resources_size;
};
// >>>priority-based encoding gene<<<
class priorityBG
{
    public:
        typedef int priority_t;
        typedef std::vector<priority_t> gene_link_t;

        priorityBG() = default;
        priorityBG(const priorityBG &);
        priority_t& operator[](size_t loc) { return _M_gene[loc]; }
        size_t size() { return _M_gene.size(); }
        void push_back(priority_t p) { _M_gene.push_back(p); }
        static void set_max(priority_t m) { max = m; }
        void set_result(time_line::date_type r) { result=r; }
        time_line::date_type get_result() const { return result; }
        std::string to_string();
        static priority_t max;
    private:
        gene_link_t _M_gene;
        time_line::date_type result;
};
// >>>declare evaluate result<<<
class evaluate_result_t;
// >>>serial/parallel schedule generation scheme<<<
// >>>ssgs<<<
class ssgs
{
    public:
        typedef std::vector<priorityBG> population_t;
        typedef std::vector<job::number_t> topological_sort_rt;// result type
        typedef std::multimap<job::number_t,job::number_t> cut_set_t;// 割集 multimap -> to store 1 key multi times
        typedef std::pair<job::number_t,job::number_t> cut_set_elem_t;
        typedef std::map<job::resource_t,time_line::time_bulk_t> res_time_t;
        ssgs() = default;
        ssgs(std::string project_file_path);
        ssgs(std::string project_file_path,std::string log_file_path);

        void init(std::string project_file_path);
        std::vector<job::number_t> topological_sort(priorityBG);
        infor_loader::no_job_t get_all_jobs() const { return all_jobs; }
        job::resource_bulk_t get_resources() const { return resources_set; }
        bool eligible(  infor_loader::no_job_t sorted_nodes,
                        job j);
        bool eligible(  infor_loader::no_job_t sorted_nodes,
                        infor_loader::no_job_t free_nodes,
                        job::number_t no);
        cut_set_t& update_cut_set(   infor_loader::no_job_t sorted_nodes,
                                    cut_set_t& cut_set);
        void update_cut_set_partial(    infor_loader::no_job_t sorted_node,
                                        cut_set_t& cut_set,
                                        job::number_t cur_job);
        job::number_t max_priority( infor_loader::no_job_t pr_quene,
                                    priorityBG gene) const ;
        void update_priority_nodes(infor_loader::no_job_t sorted_nodes,
                                    cut_set_t cut_set,
                                    infor_loader::no_job_t& free_nodes,
                                    infor_loader::no_job_t& priority_quene);
        // job_scheduled_infor evaluate(   std::vector<job::number_t> topological_sort_result,
        //                                 priorityBG gene);
        evaluate_result_t evaluate(   std::vector<job::number_t> topological_sort_result,
                                        priorityBG gene);
        void set_time(  infor_loader::no_job_t all_jobs__,
                        std::map<job::number_t,job_scheduled_infor> schedule_infor,
                        job_scheduled_infor& jsi,
                        res_time_t& res_sl);
        time_line::date_type objective_function(priorityBG gene);
        void convert_objective_val_to_adaptive_val(
                                            std::vector<time_line::date_type>& ,
                                            time_line::date_type );
        void ssgs_sort( size_t ,
                        int ,
                        double ,
                        double ,
                        priorityBG::priority_t);
        void ssgs_sort_2( size_t ,
                        int ,
                        double ,
                        double ,
                        priorityBG::priority_t);
        population_t init_pop(  size_t pop_size,
                                priorityBG::priority_t max);
        void pop_sort(population_t& );
        bool gene_cmp(priorityBG , priorityBG );
        bool adaptive_cmp(priorityBG ,priorityBG ,population_t );
        population_t select_parents(population_t pop);
        population_t crossover(population_t parents,double crossover_rate);
        population_t crossover_2(population_t parents,double crossover_rate);
        void mutate(priorityBG& ind,double mutate_rate,int neighborhood_with = 4);
        int roulette_wheel(std::vector<time_line::date_type> );
        void add_children_to_pop(population_t& pop,population_t::value_type c);
        void add_children_to_pop(population_t& pop,population_t c);
        double get_rand_val_0_to_1();

        // quik_sort_for_pop_sort
        int quik_sort_partition(population_t& pop,
                                std::vector<time_line::date_type>& val_ls,
                                int first,
                                int end);
        void quik_sort(population_t& pop,
                        std::vector<time_line::date_type>& val_ls,
                        int first,
                        int end);
        
        // log file
        void set_log_file(std::string log_file);
        // void write_in(std::string log_infor);
        void set_log_buffer_size(size_t buffer_size);
        void write_in(std::string str) { log.wirte_in(str);}
    private:
        infor_loader::no_job_t all_jobs;// all the jobs in the project
        job::resource_bulk_t resources_set;// limited resources
        priorityBG best_result;//
        _M_th::text_writer log;// log writer
};
// void ssgs(std::string project_file_path);
// >>>evaluate result<<<
class evaluate_result_t
{
    public:
        evaluate_result_t(job_scheduled_infor jsi_,
                            ssgs::res_time_t rtl,
                            std::map<job::number_t,
                                        job_scheduled_infor> si
                            ):  jsi(jsi_),
                                resources_time_line(rtl),
                                scheduled_infor(si){}
        job_scheduled_infor get_jsi() const { return jsi; }
        void print(std::ostream&);
        std::string scheduled_infor_to_string();
    private:
        job_scheduled_infor jsi;
        ssgs::res_time_t resources_time_line;
        std::map<job::number_t,job_scheduled_infor> scheduled_infor;
};
void psgs();

// test 
class test
{
    public:
        static void infor_loader_test();
        static void priorityBG_test();
        static void ssgs_test();
        static void ssgs_update_cut_set_test();
        static void unordered_map_test();
        static void topological_sort_test();
        static void iterator_test();
        static void condition_test();
        static void evaluate_test();
        static void quik_sort_test();
        static void select_parents_test();
        static void ssgs_sort_test(std::string);
        static void load_dot_RCP_file_test();
        static void ssgs_sort_dot_RCP_file_test();
};
} // namespace RCPSP

# endif