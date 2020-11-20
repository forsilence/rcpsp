# include"../include/text_handler.h"
# include<sstream>
# include<fstream>
# include<iostream>
# include<ctime>
namespace _M_th
{
    

text_loader::text_loader(std::string& text_path)
{
    load(text_path);
}
text_loader::text_loader(std::ifstream& infile)
{
    load(infile);
}

void text_loader::load(std::ifstream& infile)
{
    clear();
    if(!infile.good()){
        std::cerr << "file not exit" << std::endl;
        exit(1);
    }
    // std::ifstream infile(text_path);
    row_no_t row_no = 1;
    std::string line ;
    line_handler lh;
    while(std::getline(infile,line))
    {
        number_line_map.insert(KVE_t( row_no,line ));
        for(std::string i:lh.split_to_words(line))
        {
            words_map_t::iterator search_ptr=word_line_no.find(i);
            if(search_ptr != word_line_no.end())
                search_ptr->second.push_back(row_no);
            else word_line_no[i].push_back(row_no);
        }
        ++row_no;
    }
}
void text_loader::load(std::string& text_path)
{
    std::ifstream infile(text_path);
    if(!infile.good()) {
        std::cerr << "file " << text_path << " not exist!" << std::endl;
        exit(1);
    }
    load(infile);
}

std::ostream& text_loader::print_file(std::ostream& out)
{
    
    for(KVE_t str:number_line_map)
    {
        out << "[" ;
        out.width(3);
        out.fill('0');
        out << str.first ;
        out << "]" << str.second << std::endl;
    }
    return out ;
}
void text_loader::clear()
{
    number_line_map.clear();
    word_line_no.clear();
}
// get line by number:start by 1
text_loader::KV_t::iterator text_loader::get_line_by_no(text_loader::row_no_t no)
{
    return number_line_map.find(no);
}
text_loader::KV_t::iterator text_loader::end_of_lines()
{
    return number_line_map.end();
}
std::ostream& text_loader::print_word_line_no(std::ostream& out)
{
    for ( words_map_e_t it:word_line_no )
    {
        out << it.first << " in: " << "[" ;
        for(int line_no : it.second)
            out << line_no << " " ;
        out << "]" << std::endl;
    }
    return out;
}
// << -- line_handler -- >> 
line_handler::line_handler(std::string line)
{
    split_to_words(line);
}

line_handler::words_bulk_t line_handler::split_to_words(std::string line)
{
    if(!words.empty())words.clear();
    std::string word;
    std::istringstream line_stream(line);
    while(line_stream >> word)
        words.push_back(word);
    return words;
}

std::string line_handler::get_word_by_no(words_bulk_t::size_type no)
{
    return words.at(no);
}
// text handler
text_handler::text_handler(std::string& text_path):tl(text_path)
{
    no_line_map = tl.get_lines();
    word_line_no = tl.get_line_no();
}

text_handler::text_handler(std::ifstream& infile):tl(infile)
{
    no_line_map = tl.get_lines();
    word_line_no = tl.get_line_no();
}

void text_handler::load_text(std::string& text_path)
{
    clear();
    tl.load(text_path);
    no_line_map = tl.get_lines();
    word_line_no = tl.get_line_no();
}

void text_handler::load_text(std::ifstream& infile)
{
    clear();
    tl.load(infile);
    no_line_map = tl.get_lines();
    word_line_no = tl.get_line_no();
}

void text_handler::clear()
{
    no_line_map.clear();
    word_line_no.clear();
}

text_handler::kv_t::iterator text_handler::get_line_by_no(text_handler::row_no_t no)
{
    return no_line_map.find(no);
}

std::vector<text_handler::row_no_t> text_handler::find_word_row_no(std::string word)
{
    words_map_t::iterator tmp = word_line_no.find(word);
    std::vector<text_handler::row_no_t> tmp_vector;
    if(tmp == word_line_no.end())
        return tmp_vector;
    else return tmp->second;
}

std::vector<std::string> text_handler::split_line_to_words(std::string line)
{
    line_handler lh(line);
    return lh.get_words();
}

// >>> text_writer <<<
text_writer::text_writer(std::string file):text_writer()
                        {
                            set_file(file);
                        }

// set_file
void text_writer::set_file(std::string name)
{
    while(true)
    {
        size_t tmp = name.find(" ");
        if(tmp = std::string::npos) break;
        else if(tmp=0) name = name.substr(1,name.size()-1);
        else if(tmp=name.size()-1) name = name.substr(0,name.size()-1);
        else
        {
            std::cerr << "file can not have \" \" , you should set name as consecutive char!" << std::endl;
            exit(1);
        }
    }
    file_name = name;
}

// wirte_in
// @arg std::string
void text_writer::wirte_in(std::string text)
{
    if(buffer_pool.size()>=buffer_size)
        in_to_file();
    buffer_pool.push_back(text);
}
// destructor
text_writer::~text_writer()
{
    if(!file_name.empty())
        in_to_file();
}
// in_to_file (write buffer_pool into file)
void text_writer::in_to_file()
{
    std::ofstream outfile;
    if(file_name.empty())
    {
        std::cerr << "You have not set the file to write in,please set the file!" << std::endl;
        exit(1);
    }
    outfile.open(file_name,std::ofstream::app);
    for(buffer_type it:buffer_pool)
        outfile << it << std::endl;
    outfile.close();
    buffer_pool.clear();
}

// >>> test <<<
void test::text_writer_test()
{
    srand((unsigned)time(NULL));
    std::cout << ">>> text_writer test <<<" << std::endl;
    text_writer tw("text_writer_test");
    std::string str_table("abc defg hijklmnopqrstuvwxyzABCDEFJHIJKLMNOPQRSTU VWXYZ ");
    for(int i=0;i<40;i++)
    {
        std::string tmp;
        for(std::string::value_type it:str_table)
        if(rand()/10 > 5)
            tmp.push_back(it);
        std::cout << tmp <<std::endl;
        tw.wirte_in(tmp);
    }
    std::cout << ">>> end of text_writer test <<<" << std::endl;
}
} // namespace _M_th