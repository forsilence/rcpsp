// text-handler

# if !defined(_TEXT_HANDLER_H)
# define _TEXT_HANDLER_H
# include<string>
# include<map>
# include<iostream>
# include<vector>

namespace _M_th
{
// line in text
class line_handler
{
    public:
        typedef std::vector<std::string> words_bulk_t;
        typedef words_bulk_t::value_type value_type;
        line_handler()=default;
        line_handler(std::string line);

        words_bulk_t split_to_words(std::string line);
        words_bulk_t get_words()
        {
            return words;
        }

        // iterator related method
        words_bulk_t::iterator begin(){ return words.begin(); }
        words_bulk_t::iterator end(){ return words.end(); }
        words_bulk_t::reverse_iterator last(){  return words.rbegin(); }
        std::string get_word_by_no(words_bulk_t::size_type );
    private:
        std::vector<std::string> words;
};
// text loader
class text_loader
{
    public:
        typedef std::size_t row_no_t;
        typedef std::map<   row_no_t,
                            std::string> KV_t;
        typedef std::pair<  KV_t::key_type,
                            std::string> KVE_t;
        typedef std::map<   std::string , 
                            std::vector<row_no_t> > words_map_t;
        typedef std::pair<  std::string ,
                            std::vector<row_no_t> > words_map_e_t;
        text_loader()=default;
        text_loader(std::string &text_path);
        text_loader(std::ifstream &infile);

        void load(std::string &text_path);
        void load(std::ifstream &infile);

        KV_t get_lines() { return number_line_map; }
        words_map_t get_line_no(){ return word_line_no; }

        KV_t::iterator get_line_by_no(row_no_t);
        KV_t::iterator end_of_lines();
        std::ostream& print_file(std::ostream& out);// just test for file loader
        std::ostream& print_word_line_no(std::ostream& out);
        void clear();
    private:
        KV_t number_line_map;
        words_map_t word_line_no;
};
// text handler
class text_handler
{
    public:
        typedef text_loader::KV_t kv_t;
        typedef text_loader::KVE_t kve_t;
        typedef text_loader::row_no_t row_no_t;
        typedef text_loader::words_map_e_t words_map_e_t;
        typedef text_loader::words_map_t words_map_t;
        typedef std::vector<row_no_t> row_no_bulk_t;
        text_handler() = default;
        text_handler(std::string &text_path);
        text_handler(std::ifstream &infile);

        void load_text(std::string& text_path);
        void load_text(std::ifstream& infile);
        kv_t::iterator get_line_by_no(row_no_t no);
        kv_t::iterator end_of_lines() { return no_line_map.end(); }
        void clear();
        std::vector<row_no_t> find_word_row_no(std::string);
        std::vector<std::string> split_line_to_words(std::string line);
    private:
        text_loader tl;
        kv_t no_line_map;
        words_map_t word_line_no;
};
} // namespace _M_th
# endif