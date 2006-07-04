/*This will hold the config values, as //define statements.*/



//-----------------------------------------------------------------------------
//
// This is a SAMPLE file which you can use to specify client
// configurations and characteristics of the mail server used in your
// SPECmail tests.  You should copy this file to another file which you 
// name, and edit the properties appropriately.
//
//-----------------------------------------------------------------------------
// Configuration information
//-----------------------------------------------------------------------------

// A list of the clients which will perform load drivers for the test.
// (load generators)
#define CLIENTS "cgptest2.sentechsa.net cgptest3.sentechsa.net cgptest4.sentechsa.net"

// The host name and port number of the SMTP server
#define SMTP_SERVER "localhost"
#define SMTP_PORT 25

// The host name and port number of the POP3 server
#define POP3_SERVER "test.sentechsa.net"
#define POP3_PORT 110

// The host name and the port number of the server that will provide
// mail sink services during the benchmark
#define SINK_SERVER "cgptest1.sentechsa.net"
#define SINK_PORT 25

// The domain name that represents local mail on the mail server
#define LOCAL_DOMAIN "localdomain"

// The domain name that represents remote mail on the mail server
#define REMOTE_DOMAIN "cgptest1.sentechsa.net"

// The prefix to use on the user account names for the mailboxes;
// user account names are of the format <USERNAME_PREFIX><USER_ID>
#define USERNAME_PREFIX "user"

// A flag to indicate whether the user account name should also be 
// used as the account password (0 = no, 1 yes)
#define USERNAME_AS_PASSWORD 1

// If the user account name is not used as the account password,
// this string will be used instead.
#define USER_PASSWORD "cg8pro"

// The starting and ending numbers to use on the user account names
// for the mailboxes;  user account names are of the format 
// <USERNAME_PREFIX><USER_ID>
// USER_START 58252
// USER_END 99999
#define USER_START 1
#define USER_END 10000

// How many milliseconds to wait after sending each message sent
// during mailbox initialization
#define SMTPINIT_SEND_DELAY_MILLISECONDS 80

// How many minutes to wait after completing mailbox initialization
// to give the server a chance to deliver all the messages
#define SMTP_INIT_REST_MINUTES 12

// The name of the file to generate the results to
#define RSL_FILENAME "specmail2001.rsl"




//-----------------------------------------------------------------------------
// System Under Test (SUT) information
//-----------------------------------------------------------------------------

// The person that ran and submitted this result
#define PREPARED_BY "Philip Slater"

// The company that ran and submitted this result
#define TESTED_BY "Stalker Software"

// The company's SPEC license number
#define LICENSE_NUM "000078"

// Date the test was run
#define TEST_DATE "Jan-2005"

// The overall title of the SUT
// Note: This is used as the Title for the result and is on the index page
//       of the results published by SPEC.  It should be a description of
//        the entire system.
#define SYSTEM_TITLE "CGP Test"

// General note to appear in the results file.
#define NOTES "Notes that should appear in the results file.\n\
Any number of lines may be included in the results."

// File name of the configuration diagram for the tested system.
#define SYSTEM_CONFIG_DIAG "configdiag.gif"

// "FUNCTION" is the label that will appear at the very top of the system
// description box.  It may seem strange to have to specify an "SMTP Server",
// but it was easier to make it more generic than to have a mix of required
// machines types and free-form.
#define SYSTEM_FUNCTION "SMTP Server"

// This is the number of systems of this type used in the test.
#define SYSTEM_NUM_SYSTEMS "1"

// This is the system hardware model
// Note that this field is one of the searchable fields, so model naming
// should be consistent with other SPEC submissions
#define SYSTEM_HW_MODEL "Inspiron 3500"

// This is the system hardware vendor name
// Note that this field is one of the searchable fields, so vendor naming
// should be consistent with other SPEC submissions
#define SYSTEM_HW_VENDOR "Dell"

// Operating System (including version)" 
#define SYSTEM_OS "HURL 6.1 (kernel 2.2.15pre8)"

// The file system used by the mailserver
#define SYSTEM_FILESYSTEM "ext2"

// SW_LABEL is simply the label that appears in the Software section. 
// The only special value for this field is "JVM" (in any case); this 
// causes the SW_NAME value to be ignored, and the JVM and JIT fields 
// to be used instead.
#define SYSTEM_SW_LABEL "SMTP Software"

// Name and version number of the mailserver software
#define SYSTEM_SW_NAME "Mailserver 2001"

// Date when the mail server software is shipping and generally available
// to the public
#define SYSTEM_SW_AVAIL "Jan-2001"

// Uncomment and fill in the JVM & JIT description and version only if the
// SYSTEM_SW_LABEL field is JVM
//#define SYSTEM_JVM ""
//#define SYSTEM_JIT ""

// What type of processor(s) the system has
// **IMPORTANT**  This is the field that's used to determine whether or not
// a system description exists.  YOU MUST SET THIS FOR ALL SYSTEMS!
#define SYSTEM_CPU "Celeron"

// MegaHertz rating of the chip.
// This field is treated as a string, so Intel can list their 600BE
// megacycles/second processors.
#define SYSTEM_CPUMHZ "366"

// The number of CPUs in the system.
#define SYSTEM_CPU_ENABLED "1"

// Date when the mail server hardware is shipping and generally available
// to the public
#define SYSTEM_HW_AVAIL "Jan-2001"

// Amount of physical memory in the system, in Megabytes. 
// DO NOT USE MB OR GB, IT WILL CONFUSE THE REPORTER.
#define SYSTEM_MEMORY 128

// Amount of level 1 cache, for instruction (I) and data (D) on each CPU
#define SYSTEM_L1CACHE "16KB(I) + 16KB(D) on chip"

// Amount of level 2 cache on each CPU
#define SYSTEM_L2CACHE "128KB"

// Amount of level 3 cache (or above) on each CPU
#define SYSTEM_L3CACHE "N/A"

// Size and type of disk(s) used by the mail server
#define SYSTEM_DISK "1 x 10GB SCSI"

// Type of network interface used by the mail server
#define SYSTEM_NETWORK[0] "Loopback interface"

// Any other hardware you think is performance-related.  That is,
// you need this hardware to reproduce the test
#define SYSTEM_HW_OTHER "N/A"

// Even if you only have one line of notes, it's still [n][0]. 
// Specifying SYSTEM_NOTES[0] "foo" just won't work.
//#define SYSTEM_NOTES[0][0] "These are notes for a specific piece of hardware/software."
//#define SYSTEM_NOTES[0][1] "Again, mulitlpe lines are allowed if desired."

//
// The following fields are duplicates of the above for additional systems.
// system[1] is a description of another peice of the mail server
//           (assumes that the POP3 mail server is separate from the SMTP server)
// system[2] is a description of a load generator
//
#define SYSTEM_HW_MODEL_1 "Inspiron 3500"
#define SYSTEM_HW_VENDOR_1 "Dell"
#define SYSTEM_FUNCTION_1 "POP Server"
#define SYSTEM_NUM_SYSTEMS_1 "1"
#define SYSTEM_OS[1] "HURL 6.1 (kernel 2.2.15pre8)"
#define SYSTEM_FILESYSTEM[1] "ext2"
#define SYSTEM_SW_LABEL[1] "POP Software"
//#define SYSTEM_JVM[1] "Blackdown"
//#define SYSTEM_JIT[1] "xxx"
#define SYSTEM_SW_NAME[1] "qpopper"
#define SYSTEM_SW_AVAIL[1] "Jan-2001"
#define SYSTEM_CPU[1] "Celeron"
#define SYSTEM_CPUMHZ[1] "366"
#define SYSTEM_CPU_ENABLED[1] "1"
#define SYSTEM_HW_AVAIL[1] "Jan-2001"
#define SYSTEM_L1CACHE[1] "16KbI + 16KbD"
#define SYSTEM_L2CACHE[1] "128Kb Unified On-Chip"
#define SYSTEM_L3CACHE[1] "N/A"
#define SYSTEM_MEMORY[1] 128
#define SYSTEM_DISK[1] "1 x 10GB"
#define SYSTEM_NETWORK[1] "Loopback interface"
#define SYSTEM_HW_OTHER[1] "N/A"
#define SYSTEM_NOTES[1][0] "These are notes for a specific piece of hardware/software."
#define SYSTEM_NOTES[1][1] "Again, mulitlpe lines are allowed if desired."

//
// Here is an example of the Load Generator
//
#define SYSTEM_HW_MODEL[2] "Inspiron 3500"
#define SYSTEM_HW_VENDOR[2] "Dell"
#define SYSTEM_FUNCTION[2] "Load Generator"
#define SYSTEM_NUM_SYSTEMS[2] "1"
#define SYSTEM_OS[2] "HURL 6.1 (kernel 2.2.15pre8)"
#define SYSTEM_FILESYSTEM[2] "ext2"

// The sw_label for any of the systems running java as its primary software
// need to set SW_LABEL to "JVM".
#define SYSTEM_SW_LABEL[2] "JVM"
#define SYSTEM_JVM[2] "Blackdown"
#define SYSTEM_JIT[2] "xxx"

#define SYSTEM_SW_NAME[2] "JAVA"
#define SYSTEM_SW_AVAIL[2] "Jan-2001"
#define SYSTEM_CPU[2] "Celeron"
#define SYSTEM_CPUMHZ[2] "366"
#define SYSTEM_CPU_ENABLED[2] "1"
#define SYSTEM_HW_AVAIL[2] "Jan-2001"
#define SYSTEM_L1CACHE[2] "16KbI + 16KbD"
#define SYSTEM_L2CACHE[2] "128Kb Unified On-Chip"
#define SYSTEM_L3CACHE[2] "N/A"
#define SYSTEM_MEMORY[2] 128
#define SYSTEM_DISK[2] "1 x 10GB"
#define SYSTEM_NETWORK[2] "Loopback interface"
#define SYSTEM_HW_OTHER[2] "N/A"
#define SYSTEM_NOTES[2][0] "These are notes for a specific piece of hardware/software."
#define SYSTEM_NOTES[2][1] "Again, mulitlpe lines are allowed if desired."

//-----------------------------------------------------------------------------
//
// Log/Report related variables
// 
// Change the following variables will have performance impact on the 
// client. These trace flags should all be commented out when running
// the benchmark to produce pubilshable results.
//
//-----------------------------------------------------------------------------
//
// Each of the following controls some of the debug/diagnostic information
// printed by the benchmark. This is mainly useful in debugging the benchmark
// itself and using the benchmark as a diagnostic tool
//

// This one gives a message for each event initiated.
#define TRACE_EVENTS 1

//#define  TRACE_POP3 1
//#define  TRACE_POP3VRFY 1
//#define  TRACE_POP3CLEAN 1
//#define  TRACE_SMTPINIT 1
//#define  TRACE_SMTPQOS 1
//#define  TRACE_SMTP 1
//#define  TRACE_SMTPSINK 1
//#define  TRACE_MISC 1
//#define  TRACE_MDIM_KEYS 1
//#define  TRACE_CLIENT_MSGS 1
//#define  TRACE_SETDESTPERCENT 1
//#define  TRACE_SETMSGSIZES 1
//#define  TRACE_SETMBOXSIZES 1
//#define  TRACE_CONFIGREAD 1
//#define  TRACE_READFROM 1
//#define  TRACE_SETRECIP_MSGS 1
//#define  TRACE_MODEM_RATES 1
//#define  TRACE_CALCRESULT 1
//#define  TRACE_GETFORMATTEDRESULT 1
//#define  TRACE_RESULTS 1
//#define  TRACE_MSGSIZE 1
//#define  TRACE_WORKLOAD 1
//#define  TRACE_RETRYPOP 1
//#define  TRACE_VERIFY_MBOX_COUNT 1
//#define  TRACE_VERIFY_MSG_SIZE 1

//-----------------------------------------------------------------------------
//
// OVERRIDES
// Values that override standard fixed parameters
//
//-----------------------------------------------------------------------------

// These sample overrides make a short run.
//#define WARMUP_SECONDS 600
//#define RUN_SECONDS 600
//#define RAMPDOWN_SECONDS 600
//#define LOAD_FACTORS "100%"


