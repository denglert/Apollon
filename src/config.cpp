/**
 * config.cc  Implements tools for reading config files.
 */


#include "config.h"


//////////////////// Implemetation of class config_entry ///////////////


config_entry::config_entry()
 : string_(), longdouble_(0.0), success_(false)
{
}


config_entry::config_entry(const config_entry &other)
 : string_(other.string_), longdouble_(other.longdouble_), success_(other.success_)
{
}


config_entry& config_entry::reinit(const std::string &str)
{
    string_=str;
    std::istringstream iss(string_);
    long double zero=0.0;
    long double minusone=-1.0;
    if ( !(iss>>longdouble_) )
    {
	if ( string_=="inf" ) { longdouble_=1.0/zero; success_=true; }
	else if ( string_=="-inf" ) { longdouble_=-1.0/zero; success_=true; }
	else if ( string_=="nan" ) { longdouble_=::sqrtl(minusone); success_=true; }
	else { longdouble_=0.0; success_=false; }
    }
    else { success_=true; }
    return *this;
}


config_entry::config_entry(const std::string &str)
 : string_(), longdouble_(0.0), success_(false)
{
    reinit(str);
}


config_entry::~config_entry()
{
}


config_entry::operator std::string() const
{
    return string_;
}


config_entry::operator float() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as float.\n[config]\tconfig_entry::operator float() const\n";
    return (float)longdouble_;
}


config_entry::operator double() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as double.\n[config]\tconfig_entry::operator double() const\n";
    return (double)longdouble_;
}


config_entry::operator long double() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as long double.\n[config]\tconfig_entry::operator long double() const\n";
    return longdouble_;
}


config_entry::operator int() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as int.\n[config]\tconfig_entry::operator int() const\n";
    const int result=(int)longdouble_;
    if ( (long double)result!=longdouble_ ) std::cerr<<"[config] Problems while interpreting entry \""<<string_<<"\" as int.\n[config]\tconfig_entry::operator int() const\n";
    return result;
}


config_entry::operator unsigned int() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as unsigned int.\n[config]\tconfig_entry::operator unsigned int() const\n";
    const unsigned int result=(unsigned int)longdouble_;
    if ( (double)result!=longdouble_ ) std::cerr<<"[config] Problems while interpreting entry \""<<string_<<"\" as unsigned int.\n[config]\tconfig_entry::operator unsigned int() const\n";
    return result;
}


config_entry::operator bool() const
{
    if ( !success_ ) std::cerr<<"[config] Could not interpret entry \""<<string_<<"\" as bool.\n[config]\tconfig_entry::operator bool() const\n";
    const bool result=(bool)longdouble_;
    if ( (long double)result!=longdouble_ ) std::cerr<<"[config] Problems while interpreting entry \""<<string_<<"\" as bool.\n[config]\tconfig_entry::operator bool() const\n";
    return result;
}


config_entry::operator char() const
{
    if ( string_.empty() ) return '\0';
    if ( string_.length()>1 ) std::cerr<<"[config] String \""<<string_<<"\" consists of more than 1 characters.\n[config]\tconfig_entry::operator char() const\n";
    return string_[0];
}


std::string config_entry::string() const
{
    return string_;
}


long double config_entry::ldbl() const
{
    return longdouble_;
}


bool config_entry::success() const
{
    return success_;
}


const config_entry& config_entry::operator=(const config_entry &other)
{
    string_=other.string_;
    longdouble_=other.longdouble_;
    success_=other.success_;
    return *this;
}


//////////////////// Implementation of class config ////////////////////

config::config()
 : std::map< std::string, config_entry >()
{
}


config::config(const config &other)
 : std::map< std::string, config_entry >(other)
{
}


config::~config()
{
}


config& config::append(std::istream &file)
{
    std::string linebuff;
    std::istringstream iss;
    std::string token, value_str, buff;
    while ( std::getline(file, linebuff) )
    {
	if ( linebuff.length()==0 ) continue;
	if ( linebuff[0]=='#' ) continue;
	for ( unsigned int i=0 ; i<linebuff.length() ; ++i )
	{
	    if ( linebuff[i]=='=' ) linebuff.replace(i, 1, " ");
	}
	iss.str(linebuff);
	if ( !(iss>>token) ) { iss.clear(); continue; }
	if ( !(iss>>value_str) )
	{
	    std::cerr<<"[config] Could not extract config value from line:\n[config] "<<linebuff<<"\n[config]\nconfig& config::append(std::ifstream&)\n";
	    iss.clear();
	    continue;
	}
	if ( (iss>>buff) )
	{
	    std::cerr<<"[config] An other entry is also present after config value in line:\n[config] "<<linebuff<<"\n[config]\tconfig& config::append(std::ifstream&)\n";
	}
	iss.clear();
	(*this)[token]=config_entry(value_str);
    }
    return *this;
}


config& config::append(const std::string &filename)
{
    std::istream *file=std::openin(filename);
    if ( !(*file) )
    {
	std::cerr<<"[config] Could not open config file "<<filename<<" !\n[config]\nconfig& config::append(const std::string&)";
	delete file;
	return *this;
    }
    append(*file);
    delete file;
    return *this;
}


config::config(std::istream &file)
 : std::map< std::string, config_entry >()
{
    append(file);
}


config::config(const std::string &filename)
 : std::map< std::string, config_entry >()
{
    append(filename);
}


config& config::clear()
{
    std::map< std::string, config_entry >::clear();
    return *this;
}


//////////////////// Implementation of getconfig functions /////////////


config_entry getconfig(config &conf, const std::string &token)
{
    if ( conf[token].string_.empty() )
    {
	std::cerr<<"[config] "<<token<<" is not specified in configfile!\n[config]\tconfig_entry getconfig(config&, const std::string&)\n";
    }
    return conf[token];
}


config_entry getconfig(std::istream &in, const std::string &token)
{
    config conf(in);
    return getconfig(conf, token);
}


config_entry getconfig(const std::string &filename, const std::string &token)
{
    config conf(filename);
    return getconfig(conf, token);
}


//////////////////// Implementation of filenumbering function //////////


std::string filenumbering(const unsigned int no, const unsigned int maxno)
{
    int ndigits=(no!=0 ? (int)(::log10((double)no))+1 : 1);
    int ndigitsmax=(maxno!=0 ? (int)(::log10((double)maxno))+1 : 1);
    std::ostringstream oss;
    for ( int i=0 ; i<(int)ndigitsmax-(int)ndigits ; ++i ) oss<<"0";
    oss<<no;
    return oss.str();
}


//////////////////// Implementation of getrevision function ////////////


#ifndef REVISIONSTRING
#define REVISIONSTRING "unknown"
#endif
std::string getrevision()
{
    return std::string(REVISIONSTRING);
}


//////////////////// Implementation of gettime function ////////////////


unsigned int gettime()
{
    time_t TIME;
    time(&TIME);
    return (unsigned int)TIME;
}


//////////////////// Implementation of gettime_hr function /////////////


std::string gettime_hr()
{
    time_t TIME;
    char chartime[32];
    time(&TIME);
    ctime_r(&TIME, chartime);
    return std::string(chartime);
}


//////////////////// Implementation of getusertime function ////////////


unsigned int getusertime()
{
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_utime.tv_sec;
}


//////////////////// Implementation of getcputime function /////////////


unsigned int getcputime()
{
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_stime.tv_sec;
}


//////////////////// Implementation of getmem function ////////////////


unsigned int getmem()
{
    pid_t pid=getpid();
    std::ostringstream str;
    str<<"/proc/"<<pid<<"/stat";
    std::ifstream file;
    file.open(str.str().c_str());
    std::string linebuff;
    unsigned int i=0;
    while ( std::getline(file, linebuff, ' ') )
    {
	++i;
	if ( i>=23 ) break;
    }
    file.close();
    file.clear();
    std::istringstream iss(linebuff);
    unsigned int mem=0;
    if ( i<23 || !(iss>>mem) )
    {
	std::cerr<<"[config] Could not get memory!\n[config]\tunsigned int getmem()\n";
	return 0;
    }
    return mem/1048576;
}


//////////////////// Implementation of putlogdata function /////////////


void putlogdata(const std::string &filename)
{
    std::ostream *logfile=std::openout(filename);
    if ( !(*logfile) )
    {
	std::cerr<<"[config] Could not open logfile "<<filename<<" !\n[config]\tvoid putlogdata(const std::string&, const unsigned int)\n";
	delete logfile;
	return;
    }
    if ( !((*logfile)<<"Revision: "<<getrevision()<<"\n"<<"Started at: "<<summaryinfo::getstarttime_hr()<<"Running time: "<<getusertime()<<" seconds\n"<<"CPU time: "<<getcputime()<<" seconds\n"<<"Memory usage: "<<getmem()<<" MegaBytes\n"<<"Logged at: "<<gettime_hr()<<std::endl) )
    {
	std::cerr<<"[config] Could not write into logfile "<<filename<<" !\n[config]\tvoid putlogdata(const std::string&, const unsigned int)\n";
	delete logfile;
	return;
    }
    delete logfile;
}


//////////////////// Implementation of class summaryinfo ///////////////


#ifndef __NO_MEMORY_WATCHER
static std::ipstream& mkauxmaxmemstream()
{
    pid_t pid=getpid();
    if ( pid==0 )
    {
	std::cerr<<"[config] Zero pid!\n[config]\tstatic std::ipstream& mkauxmaxmemstream()\n";
    }
    std::ostringstream strpid;
    strpid<<pid;
    std::ipstream *auxmaxmemstream=new std::ipstream(((const std::string)"while (( "+strpid.str()+" )) ; do sleep 1h ; done").c_str());
    return *auxmaxmemstream;
}


static std::ipstream& mkmaxmemstream()
{
    pid_t pid=getpid();
    std::ostringstream strpid;
    strpid<<pid;
    std::ipstream *maxmemstream=new std::ipstream(((const std::string)"__MEM=0 ; __MAXMEM=0 ; while ( ps aux | grep -v \"grep\" | grep \"while (( "+strpid.str()+" ))\" >&/dev/null ) ; do __MEM=`cat /proc/"+strpid.str()+"/stat 2>/dev/null | cut -f 23 -d ' '` ; if (( $__MEM > $__MAXMEM )) ; then __MAXMEM=$__MEM ; fi ; sleep 1s ; done ; echo \"$__MAXMEM\" ; unset __MEM ; unset __MAXMEM").c_str());
    return *maxmemstream;
}
#endif




const unsigned int summaryinfo::starttime_=gettime();

const std::string summaryinfo::starttime_hr_=gettime_hr();

unsigned int summaryinfo::maxmemory_=getmem();

#ifndef __NO_MEMORY_WATCHER
std::ipstream& summaryinfo::auxmaxmemstream_=mkauxmaxmemstream();

std::ipstream& summaryinfo::maxmemstream_=mkmaxmemstream();
#endif

std::ostream& summaryinfo::logfile_=std::cerr;

std::string summaryinfo::logfilename_="";

bool summaryinfo::firstcopydeclared_=false;

bool summaryinfo::iswritten_=false;

bool summaryinfo::writelogfile_=true;


summaryinfo::summaryinfo()
 : firstcopy_(!firstcopydeclared_)
{
    firstcopydeclared_=true;
}


summaryinfo::summaryinfo(const summaryinfo &other)
 : firstcopy_(!firstcopydeclared_)
{
    firstcopydeclared_=true;
}


summaryinfo::~summaryinfo()
{
    if ( firstcopy_==true )
    {
	acquiremaxmem();
#ifndef __NO_MEMORY_WATCHER
	auxmaxmemstream_.kill();
	auxmaxmemstream_.close();
	auxmaxmemstream_.clear();
	unsigned int membuff=0;
	if ( !(maxmemstream_>>membuff) )
	{
	    std::cerr<<"[config] Could not read maximal memory stream!\n[config]\tsummaryinfo::~summaryinfo()\n";
	    membuff=0;
	}
	maxmemstream_.close();
	maxmemstream_.clear();
	membuff/=1048576;
	if ( membuff>maxmemory_ ) maxmemory_=membuff;
#endif
    }
    if ( firstcopy_==true && iswritten_==false && writelogfile_==true )
    {
	if ( !logfilename_.empty() )
	{
	    std::ostream *logfile=std::openout(logfilename_);
	    if ( !(*logfile) )
	    {
		std::cerr<<"[config] Could not open logfile "<<logfilename_<<" !\n[config]\tsummaryinfo::~summaryinfo()\n";
		delete logfile;
		iswritten_=true;
		return;
	    }
	    if ( !((*logfile)<<"Started at: "<<getstarttime_hr()<<"Running time: "<<getusertime()<<" seconds\n"<<"CPU time: "<<getcputime()<<" seconds\n"<<"Memory usage: "<<maxmemory_<<" MegaBytes\n"<<"Ended at: "<<gettime_hr()<<std::endl) )
	    {
		std::cerr<<"[config] Could not write into logfile "<<logfilename_<<" !\n[config]\tsummaryinfo::~summaryinfo()\n";
		delete logfile;
		iswritten_=true;
		return;
	    }
	    delete logfile;
	    iswritten_=true;
	}
	else
	{
	    if ( !(logfile_) )
	    {
		std::cerr<<"[config] Could not open logfile!\n[config]\tsummaryinfo::~summaryinfo()\n";
		iswritten_=true;
		return;
	    }
	    if ( !(logfile_<<"Started at: "<<getstarttime_hr()<<"Running time: "<<getusertime()<<" seconds\n"<<"CPU time: "<<getcputime()<<" seconds\n"<<"Memory usage: "<<maxmemory_<<" MegaBytes\n"<<"Ended at: "<<gettime_hr()<<std::endl) )
	    {
		std::cerr<<"[config] Could not write into logfile!\n[config]\tsummaryinfo::~summaryinfo()\n";
		iswritten_=true;
		return;
	    }
	    iswritten_=true;
	}
    }
}


const summaryinfo& summaryinfo::operator=(const summaryinfo &other)
{
    return *this;
}


void summaryinfo::logfilename(const std::string &filename)
{
    logfilename_=filename;
    acquiremaxmem();
}


std::string summaryinfo::logfilename()
{
    acquiremaxmem();
    return logfilename_;
}


void summaryinfo::write_logfile(const bool flag)
{
    writelogfile_=flag;
    acquiremaxmem();
}


bool summaryinfo::write_logfile()
{
    acquiremaxmem();
    return writelogfile_;
}


unsigned int summaryinfo::getstarttime()
{
    return starttime_;
}


std::string summaryinfo::getstarttime_hr()
{
    return starttime_hr_;
}


void summaryinfo::acquiremaxmem()
{
    unsigned int memory=getmem();
    if ( maxmemory_<memory ) maxmemory_=memory;
}


unsigned int summaryinfo::showmaxmem()
{
    acquiremaxmem();
    return maxmemory_;
}


#ifndef __NO_AUTO_LOGGING
const summaryinfo __summaryinfo;
#endif

