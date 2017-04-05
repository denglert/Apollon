/**
 * config.h  Declares tools for reading config files, consisting of
 *           lines of the format:
 *           token      value
 *           pairs. Empty lines, lines containing only delimiters
 *           (tabs and spaces), and lines beginning with '#' are
 *           ignored.
 *           A configfile is read by:
 *           // Declare config variable:
 *           config conf;
 *           // Read the content of a configfile into conf:
 *           conf.append("configfile");
 *           // Get the value associated to "token", interpreted
 *           // explicitely as of type T:
 *           T t=getconfig<T>(conf, "token");
 *           // (In case of failure, automatic warning is displayed.)
 *           // One may omit the template argument, to interpret the
 *           // value automatically (implicitly) of the appropriate
 *           // type:
 *           T t=getconfig(conf, "token");
 *           // Clear the content of the config variable:
 *           conf.clear();
 *           There are also useful system resource usage functions.
 */


#ifndef __CONFIG_H
#define __CONFIG_H


// Uncomment this if you don not want to launch memory watcher 
// satellite program:
#define __NO_MEMORY_WATCHER


// Uncomment this if you don not want automatic logging 
// when program finishes:
//#define __NO_AUTO_LOGGING


#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <pstream.h>


// Forward declaration of class config_entry. This stores a config file 
// entry.
class config_entry;


// Forward declaration of class config. This stores a config file 
// content (an array of config_entry variables).
class config;


// Forward declaration of template function getconfig. This extracts an 
// entry from a config variable, indexed by a token, explicitly 
// interpreted as of a given specified type.
template <typename T>
T getconfig(config&, const std::string&);
// The same, but reads from file, specified by an input stream.
template <typename T>
T getconfig(std::istream&, const std::string&);
config_entry getconfig(std::istream&, const std::string&);
// The same, but reads from file, specified by a file name.
template <typename T>
T getconfig(const std::string&, const std::string&);

// Forward declaration of function getconfig. This extracts an entry 
// from a config variable, indexed by a token, but does not interpret 
// it in a forced way (automatic conversion will interpret it as some 
// type later, implicitly).
extern config_entry getconfig(config&, const std::string&);
// The same, but reads from file, specified by an input stream:
extern config_entry getconfig(std::istream&, const std::string&);
// The same, but reads from file, specified by a file name:
extern config_entry getconfig(const std::string&, const std::string&);


/**
 * Stores a config file entry, which can either contain a string
 * or a number (long double, int, unsigned int, bool), or a char.
 */
class config_entry
{
    protected:
	/// Original string value:
	std::string string_;
	/// string_ interpreted as long double:
	long double longdouble_;
	/// string -> long double conversion successfullness flag:
	bool success_;
    public:
	/// Default constructor:
	config_entry();
	/// Copy constructor:
	config_entry(const config_entry&);
	/// Reinitialization from an std::string:
	config_entry& reinit(const std::string&);
	/// Constructor from an std::string:
	config_entry(const std::string&);
	/// Destructor:
	~config_entry();
	/// Implicit conversion to std::string:
	operator std::string() const;
	/// Implicit conversion to float:
	operator float() const;
	/// Implicit conversion to double:
	operator double() const;
	/// Implicit conversion to long double:
	operator long double() const;
	/// Implicit conversion to int:
	operator int() const;
	/// Implicit conversion to unsigned int:
	operator unsigned int() const;
	/// Implicit conversion to bool:
	operator bool() const;
	/// Implicit conversion to char:
	operator char() const;
	/// Return the string data field:
	std::string string() const;
	/// Return the long double data field:
	long double ldbl() const;
	/// Return a flag, which tells if initial string -> long double conversion was successful:
	bool success() const;
	/// Assignment:
	const config_entry& operator=(const config_entry&);
	/// Friend function getconfig:
	friend config_entry getconfig(config&, const std::string&);
};


/**
 * Declaration of a config file content container (class config).
 */
class config : protected std::map< std::string, config_entry >
{
    public:
	/// Default constructor:
	config();
	/// Copy constructor:
	config(const config&);
	/// Destructor:
	~config();
	/// Append the content of a config file:
	config& append(std::istream&);
	/// Append the content of a config file, specified by the name:
	config& append(const std::string&);
	/// Constructor with the content of a config file:
	config(std::istream&);
	/// Constructor with the content of a config file, specified by the name:
	config(const std::string&);
	/// Clear content:
	config& clear();
	/// Friend function getconfig:
	friend config_entry getconfig(config&, const std::string&);
};


/**
 * Implementation of getconfig<> template functions.
 */
template <typename T>
T getconfig(config &conf, const std::string &token)
{
    return (T)getconfig(conf, token);
}

template <typename T>
T getconfig(std::istream &in, const std::string &token)
{
    return (T)getconfig(in, token);
}

template <typename T>
T getconfig(const std::string &filename, const std::string &token)
{
    return (T)getconfig(filename, token);
}

// See the implementation of getconfig functions without template 
// arguments in config.cc.


/**
 * File numbering function. Extends the front digits with appropriate 
 * number of zeros. (Arguments: index, maximal index):
 */
extern std::string filenumbering(const unsigned int, const unsigned int);


/**
 * Get the revision string.
 */
extern std::string getrevision();


/**
 * Get the elapsed time in seconds since the Epoch 
 * (00:00:00.00 UTC, 1 Jauary 1970):
 */
extern unsigned int gettime();


/**
 * Get the actual time and date in a human-readable string format:
 */
extern std::string gettime_hr();


/**
 * Get the user time of the in seconds, used up so far by the program:
 */
extern unsigned int getusertime();


/**
 * Get the actual CPU time in seconds, used up so far by the program:
 */
extern unsigned int getcputime();


/**
 * Get currently used memory in MegaBytes:
 */
extern unsigned int getmem();


/**
 * Write logging data into file:
 */
extern void putlogdata(const std::string&);


/**
 * This class stores program summary information (starting time etc.):
 */
class summaryinfo
{
    // Private, not protected, so user cannot use these, 
    // even with inherited class:
    private:
	/// Starting time in seconds from the Epoch (00:00:00.00 UTC, 1 Jauary 1970):
	static const unsigned int starttime_;
	/// Starting time in human-readable format:
	static const std::string starttime_hr_;
	/// Current maximal used memory in MegaBytes:
	static unsigned int maxmemory_;
#ifndef __NO_MEMORY_WATCHER
	/// An auxiliary stream for communicating with the below stream:
	static std::ipstream& auxmaxmemstream_;
	/// Maximal memory reading stream:
	static std::ipstream& maxmemstream_;
#endif
	/// Logfile stream:
	static std::ostream &logfile_;
	/// Logfile name:
	static std::string logfilename_;
	/// Determine if this is the first copy of this class:
	const bool firstcopy_;
	static bool firstcopydeclared_;
	/// Determine if already written:
	static bool iswritten_;
	/// Write/not write logfile:
	static bool writelogfile_;
    public:
	/// Default constructor:
	summaryinfo();
    private:
	/// Copy constructor (so that user cannot call it):
	summaryinfo(const summaryinfo&);
    public:
	/// Destructor (writes summary logfile, if requested):
	~summaryinfo();
    private:
	/// Assignment operator (so that user cannot call it):
	const summaryinfo& operator=(const summaryinfo&);
    public:
	/// Change the logfile stream (cerr by default):
	static void logfile(const std::ostream&);
	/// Get the logfile stream:
	static std::ostream& logfile();
	/// Change the name of logfile ("", directed to cerr by default):
	static void logfilename(const std::string&);
	/// Get the name of logfile:
	static std::string logfilename();
	/// Require/not require logfile writing (require by default):
	static void write_logfile(const bool);
	/// Determine if logfile is required:
	static bool write_logfile();
	/// Get start time:
	static unsigned int getstarttime();
	/// Get start time in human-readable format:
	static std::string getstarttime_hr();
	/// Acquire maximal memory:
	static void acquiremaxmem();
	/// Show used maximal memory:
	static unsigned int showmaxmem();
};


#ifndef __NO_AUTO_LOGGING
/**
 * Summary info variable. The constructor of this will record the 
 * starting time of the program. The destructor of this will write a 
 * summary logfile.
 */
extern const summaryinfo __summaryinfo;
#endif


#endif /* __CONFIG_H */
