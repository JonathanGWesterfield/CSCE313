/*
    File: client.C

    Author: Ya boi its me. Jabroni.
            Department of Computer Science
            Texas A&M University
    Date  : Does it matter?

    Client main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <iomanip> //setw

#include <errno.h>
#include <unistd.h>

#include "reqchannel.H"
#include "BoundedBuffer.h"
#include "FileHandler.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* function prototypes */
/*--------------------------------------------------------------------------*/

// RequestChannel *createControlChannel();
void createControlChannel(); // opens up dataserver path
void* requestData(void* paramName); // request thread function
void* worker_thread(void* requestChannel); // communicates between buffer and dataserver
void* statisticsThread(void* bufferName); // statistics thread
void printHistogram(vector<int> data, string name, int personID);
void print_time_diff(struct timeval * tp1, struct timeval * tp2);
// prints out the histogram vectors and also outputs to an excel ".xls" file

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

BoundedBuffer *requestBuffer; // main request buffer
BoundedBuffer * joeBuff; // statistics buffers
BoundedBuffer * janeBuff;
BoundedBuffer * johnBuff;

vector<int> joeHist(100); // histograms for the 3 people
vector<int> janeHist(100);
vector<int> johnHist(100);
pthread_mutex_t m; // mutex to interact with the histograms

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

const string joe = "data Joe";
const string jane = "data Jane";
const string john = "data John";

const int * joeID = new int(0); // Number ID's for Joe, Jane and John
const int * janeID = new int(1);
const int * johnID = new int(2);

bool startThreads = false; // flag for threads to start
bool killYourself = false; // flag for worker threads to die

Response * startSignal = new Response("Start", 999, 999);
Response * killSignal = new Response("Kill", -999, -999);

/** Number of Requests and threads and such */
int nRequests = 10000; //program arguments and defaults
int wThreads = 15;
int bufferSize = 5000;
RequestChannel* channel; // first and default request channel

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
/**
 * @brief generic error message to call if a thread fails
 * @param msg
 */
void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

// starts the dataserver so that the client can run
void startDataServer()
{
  pid_t pid = fork();

  // check to make sure that forking was successful
    if (pid == -1)
    {
        fprintf(stderr, "Fork of process failed: %s\n", strerror(errno));
        exit(0); // get the hell outta there
    }

    if(pid == 0) // child process
    {
      printf("Starting Server\n\n");
      execv("dataserver", NULL);
    }
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

/**
 * @brief main function. It takes in the program arguements and stores them
 *      into the appropriate global variables. From there, starts the server,
 *      creates the connection to the server, and starts up the
 *      requestData, WorkerThread and statisticsThread in the typical
 *      concurrent way. It then disconnects from the server by sending the
 *      quit signal (which destroys the servers process) and then calls
 *      a print function to output the histogram.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char * argv[])
{
    // gets the program arguments
    int c = 0;
    while((c = getopt (argc, argv, "n:w:b:")) != -1)
    {
        switch(c)
        {
            case 'n':
                nRequests = atoi(optarg);
                break;
            case 'w':
                wThreads = atoi(optarg);
                break;
            case 'b':
                bufferSize = atoi(optarg);
                break;
            case '?':
                printf("Unknown option - aborting\n\n");
                exit(0);
                abort();
            default:
                printf("cmd line error\n");
                printf("options -n # -w # -b #\n\n");
                exit(0);
                // abort();
        }
    }

    pthread_mutex_init(&m, NULL); // intialize our mutex lock

    createControlChannel();

    // have arguments now initialize
    pthread_t requestThreads[3];// one for each person 0=joe 1=jane 2=john
    pthread_t workerThreads[wThreads];
    pthread_t statThreads[3];

    requestBuffer = new BoundedBuffer(bufferSize); // main request buffer
    joeBuff = new BoundedBuffer(bufferSize); // statistics buffers
    janeBuff = new BoundedBuffer(bufferSize);
    johnBuff = new BoundedBuffer(bufferSize);

    struct timeval tp_start; /* Used to compute elapsed time. */
    struct timeval tp_end;

    assert(gettimeofday(&tp_start, 0) == 0);


    // creates and runs the request threads
    if(pthread_create(&requestThreads[0], NULL, requestData, (void*)joeID) == -1) // thread, NULL, function, function args
        error((char*)"Can't create thread for Joe. Fuck Joe");
        // fprintf(stderr, "%s: %s\n", "Can't create thread for Joe. Fuck Joe", strerror(errno));
    if(pthread_create(&requestThreads[1], NULL, requestData, (void*)janeID) == -1)
        error((char*)"Can't create thread for Jane. Fuck Jane");
        // fprintf(stderr, "%s: %s\n", "Can't create thread for Jane. Fuck Jane", strerror(errno));
    if(pthread_create(&requestThreads[2], NULL, requestData, (void*)johnID) == -1)
        error((char*)"Can't create thread for John. Fuck John");
        // fprintf(stderr, "%s: %s\n", "Can't create thread for John. Fuck John", strerror(errno));

    // creates the specified number of worker threads
    for(int i = 0; i < wThreads; i++) // workerThreads.size(); i++)
    {
        string reply = channel->send_request("newthread"); // tells server to create a new thread for a new connection
        // creates a new RequestChannel object for each new worker thread
        printf("Reply to newthread %d is : %s\n", i, reply.c_str());
        RequestChannel* workerChannel = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);

        /** creates a new worker thread and giving the thread a new requestChannel to use to
         * talk to the server */
        if(pthread_create(&workerThreads[i], NULL, worker_thread, (void*)workerChannel) == -1)
            error((char*)"Can't create worker thread. main()");
            // fprintf(stderr, "%s: %s\n", "Can't create worker thread. main()", strerror(errno));
    }

    /** Creates all of our statistics threads */
    printf("Creating stat threads\n");
    if(pthread_create(&statThreads[0], NULL, statisticsThread, (void*)joeID) == -1) // thread, NULL, function, function args
        error((char*)"Can't create statistics thread for Joe. Fuck Joe");
        //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for Joe. Fuck Joe", strerror(errno));
    if(pthread_create(&statThreads[1], NULL, statisticsThread, (void*)janeID) == -1)
        error((char*)"Can't create statistics thread for Jane. Fuck Jane");
        //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for Jane. Fuck Jane", strerror(errno));
    if(pthread_create(&statThreads[2], NULL, statisticsThread, (void*)johnID) == -1)
        error((char*)"Can't create statistics thread for John. Fuck John");
        //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for John. Fuck John", strerror(errno));

    //TODO: use the generic error function for the rest of the thread calls
    /** Join all of the request threads with error messages just in case */
    if (pthread_join(requestThreads[0], NULL) == -1)
        error((char*)"Can't join 1st request thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 1st request thread", strerror(errno));
    if (pthread_join(requestThreads[1], NULL) == -1)
        error((char*)"Can't join 2nd request thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 2nd request thread", strerror(errno));
    if (pthread_join(requestThreads[2], NULL) == -1)
        error((char*)"Can't join 3rd request thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 3rd request thread", strerror(errno));

    // sends kill signal since the request threads will be done
    requestBuffer->AddToBuffer(*killSignal);
    /** Join all of the worker threads with error messages just in case */
    for(int i = 0; i < wThreads; i++)
        if (pthread_join(workerThreads[i], NULL) == -1)
            error((char*)"Can't join worker thread");
            //fprintf(stderr, "%s: %s\n", "Can't join worker thread", strerror(errno));

    /** Join all of the worker threads with error messages just in case */
    if (pthread_join(statThreads[0], NULL) == -1)
        error((char*)"Can't join 1st statistics thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 1st statistics thread", strerror(errno));
    if (pthread_join(statThreads[1], NULL) == -1)
        error((char*)"Can't join 2nd statistics thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 2nd statistics thread", strerror(errno));
    if (pthread_join(statThreads[2], NULL) == -1)
        error((char*)"Can't join 3rd statistics thread");
        //fprintf(stderr, "%s: %s\n", "Can't join 3rd statistics thread", strerror(errno));

    assert(gettimeofday(&tp_end, 0) == 0); // gets the end timer

    /** Terminate connection to the server */
    string reply4 = channel->send_request("quit");
    printf("Reply to request 'quit' is %s\n", reply4.c_str());
    sleep(1);

    /* int sec = end.tv_sec - begin.tv_sec;
    double musec = end.tv_usec - begin.tv_usec; */

    /** print out the histograms for each person */
    printHistogram(joeHist, joe, *joeID);
    printHistogram(janeHist, jane, *janeID);
    printHistogram(johnHist, john, *johnID);

    printf("Total number of requests: %d\n", nRequests * 3);
    printf("Total number of worker threads: %d\n", wThreads);
    printf("Time taken for computation : ");
    print_time_diff(&tp_start, &tp_end);
    printf("\n");


    // printf("Total request time: %d Sec %f muSec\n", sec, musec);

    /* clears the directory of any fifo_ files */
    system((char*)"rm fifo_*");
}

/**
 * @brief inserts the name of each person into the request buffer
 * @param paramID
 * @return
 */
void* requestData(void* paramID)
{
    // personID specifies what name to put into the buffer
    int personID = *((int*)paramID); // gets the ID and turns it to an int
    requestBuffer->AddToBuffer(*startSignal); // send the start response

    /** joe, jane, john are global string variables */
    for(int i = 0; i < nRequests; i++)
    {
        if(personID == 0)
        {
            printf("Inserting %s %d Into Request Buffer\n", joe.c_str(), personID);
            requestBuffer->AddToBuffer(Response(joe, personID, i)); // sends "data Joe"
        }
        else if(personID == 1)
        {
            printf("Inserting %s %d Into Request Buffer\n", jane.c_str(), personID);
            requestBuffer->AddToBuffer(Response(jane, personID, i)); // sends "data Jane"
        }
        else if(personID == 2)
        {
            printf("Inserting %s %d Into Request Buffer\n", john.c_str(), personID);
            requestBuffer->AddToBuffer(Response(john, personID, i)); // sends John
        }
    }
    printf("REQUEST THREAD IS NOW FINISHED\n\n");
}

/**
 * @brief Will take the requests out of the request buffer and then send the
 *      name within that request to the server. Once the response from the server
 *      comes back, the thread will then put that reply into the appropriate
 *      statistics buffer
 * @param requestChannel
 * @return
 */
void* worker_thread(void* requestChannel)
{
    // gets the ReqChannel that was passed in
    RequestChannel* newChannel = (RequestChannel*)requestChannel;

    /*for(;;) // loop that waits until it gets the start response
    {   // get the first response from buffer, which will be start response
        int start = requestBuffer->RemoveFromBuffer().requestID;

        if(start == startSignal->requestID)
        {
            startThreads = true; // lets other threads know to start
            break;
        }
        else if(killYourself)
        {
            printf("Worker found killYourself = true\nKilling itself\n\n");
            pthread_exit((char*)"Buffer is already empty");
        }
        else if(startThreads) // checks startThread flag to see if should exit loop anyway
            break;
    }*/

    for(;;)
    {
        if(killYourself) // if the kill flag is true
        {
            // no point in checking the buffer; its empty
            printf("Worker found killYourself = true\nKilling itself\n\n");
            break;
        }

        Response response = requestBuffer->RemoveFromBuffer();
        int responseID = response.requestID;

        // determines if got kill signal so it should die
        if(responseID == killSignal->requestID)
        {
            pthread_mutex_lock(&m);
            killYourself = true; // set the kill flag for the other threads
            joeBuff->AddToBuffer(*killSignal); // send kill response to all statistics buffers
            janeBuff->AddToBuffer(*killSignal);
            johnBuff->AddToBuffer(*killSignal);
            pthread_mutex_unlock(&m);

            printf("Worker Received Kill Signal\nWorker now killing itself\n\n");
            break;
        }

        // get the string from the response and send it to the server
        string reply = newChannel->send_request(response.requestData); // ex. - "data Jane"
        printf("Reply to request ' %s' is ' %s'\n",
               response.requestData.c_str(), reply.c_str());

        // creates a response to send to statistics buffers
        Response *sendToStatBuffer = new Response(reply, responseID, response.count);
        // sends server reply to the statistics threads
        if(responseID == 0)
        {
            printf("Aiming at Joe's Buffer\n");
            joeBuff->AddToBuffer(*sendToStatBuffer);
            printf("Sent server reply to Joe's Buffer\n");
        }
        else if(responseID == 1)
        {
            printf("Aiming at Jane's Buffer\n");
            janeBuff->AddToBuffer(*sendToStatBuffer);
            printf("Sent server reply to Jane's Buffer\n");
        }
        else if(responseID == 2)
        {
            printf("Aiming at John's Buffer\n");
            johnBuff->AddToBuffer(*sendToStatBuffer);
            printf("Sent server reply to John's Buffer\n");
        }
    }
    printf("WORKER THREAD IS NOW DEAD\n\n");
    return NULL;
}

/**
 * @brief Takes the response from the appropriate buffer (defined by the
 *      buffer ID that is passed in. It goes to the appropriate histogram
 *      vector, and then increase the value at the index (specified by the
 *      server response) and increments it by 1
 * @param bufferID
 * @return
 */
void* statisticsThread(void* bufferID)
{
    // buffID specifies which buffer to interact with
    int buffID = *((int*)bufferID); // turns the bufferID into usable int

    /** the code for each case is nearly identical except for the person
     * that it is for. If you understand one of the cases you have the rest.
     */
    if(buffID == 0) // get from joe's buffer
    {
        for(;;)
        {
            Response newResponse = joeBuff->RemoveFromBuffer(); // get response from buffer
            int responseID = newResponse.requestID;
            string value = newResponse.requestData;
            int count = newResponse.count;

            // print out response's contents for debugging
            printf("Stat Response Thread JOE %d: %s %d\n", count, value.c_str(), responseID);

            // determines if the thread has recieved the kill code and should terminate
            if(responseID == killSignal->requestID) // is the kill code
            {
                printf("RECEIVED JOE KILL SIGNAL\n");
                break;
            }

            int index = atoi(value.c_str());
            // pthread_mutex_lock(&m);
            printf("Modifying Joe's Histogram for the %d times\n", count);
            joeHist[index]++; // increment value at that index in joe's histogram
            printf("FINISHED Modifying Joe's Histogram %d Times\n", count);
            if(count == nRequests)
                printf("Finished All of Joe's Requests!!!\n");
            // pthread_mutex_unlock(&m);
        }
    }
    else if(buffID == 1)
    {
        for(;;)
        {
            Response newResponse = janeBuff->RemoveFromBuffer();
            int responseID = newResponse.requestID;
            string value = newResponse.requestData;
            int count = newResponse.count;

            printf("Stat Response Thread JANE %d: %s %d\n", count, value.c_str(), responseID);
            if(responseID == killSignal->requestID) // is the kill code
            {
                printf("RECEIVED JANE KILL SIGNAL\n");
                break;
            }

            int index = atoi(value.c_str());
            // pthread_mutex_lock(&m);
            printf("Modifying Jane's Histogram for the %d times\n", count);
            janeHist[index]++; // increment value at that index in joe's histogram
            printf("FINISHED Modifying Jane's Histogram %d Times\n", count);
            if(count == nRequests)
                printf("Finished All of Jane's Requests!!!\n");
            // pthread_mutex_unlock(&m);
        }
    }
    else if(buffID == 2)
    {
        for(;;)
        {
            Response newResponse = johnBuff->RemoveFromBuffer();
            int responseID = newResponse.requestID;
            string value = newResponse.requestData;
            int count = newResponse.count;

            printf("Stat Response Thread JOHN %d: %s %d\n", count,  value.c_str(), responseID);
            if(responseID == killSignal->requestID) // is the kill code
            {
                printf("RECEIVED JOHN KILL SIGNAL\n");
                break;
            }

            int index = atoi(value.c_str());
            // pthread_mutex_lock(&m);
            printf("Modifying John's Histogram for the %d times\n", count);
            johnHist[index]++; // increment value at that index in joe's histogram
            printf("FINISHED Modifying John's Histogram %d Times\n", count);
            if(count == nRequests)
                printf("Finished All of John's Requests!!!\n");
            // pthread_mutex_unlock(&m);
        }
    }
    printf("STATISTICS THREAD %d IS FINISHED!!!\n\n", buffID);
    return NULL;
}

/**
 * @brief Loops through the histogram vector specified by the function args.
 *      Will output the histogram in bins that go by 10 units each. Also calls
 *      a function to export the data to an excel file to graph the data later.
 * @param data
 * @param name
 */
void printHistogram(vector<int> data, string name, int personID)
{
    // calls the Filehandler class to export the histogram data to an excel file for graphing
    FileHandler file = FileHandler(data, name, personID);
    file.printToXLS();

    vector<int> smaller(10); //make the histogram have less bars ie 0-9 10-19 20-29 etc
    for (int j = 0; j < smaller.size(); j++)
    {
        for (int k = 0; k < smaller.size(); k++)
        {
            smaller[j] += data[k + j * 10];
        }
    }
    cout<< "\n    "<< name << endl
        << setw(7) << "0-9" << setw(7) << "10-19" << setw(7) << "20-29" << setw(7) << "30-39" << setw(7)
        << "40-49" << setw(7) << "50-59" << setw(7) << "60-69" << setw(7) << "70-79" << setw(7)
        << "80-89" << setw(7) << "90-99" <<endl << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << setw(7) << smaller[i];
    }
    cout << endl << endl;

    return;
}

/**
 * @brief Initiates the connection with the server
 */
void createControlChannel()
{
    cout << "CLIENT STARTED:" << endl;
    startDataServer(); // calls the forking dataserver start function

    cout << "Establishing control channel... " << flush;
    // RequestChannel ReqChannel("control", RequestChannel::CLIENT_SIDE);
    // initiate connection with server
    channel = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;

    // initiate confirm connection with server
    string reply1 = channel->send_request("hello");
    cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
}

/**
 * @brief Timing function copied from MP1
 * @param tp1
 * @param tp2
 */
void print_time_diff(struct timeval * tp1, struct timeval * tp2)
{
    /* Prints to stdout the difference, in seconds and museconds, between
       two timevals.
    */

    long sec = tp2->tv_sec - tp1->tv_sec;
    long musec = tp2->tv_usec - tp1->tv_usec;
    if (musec < 0) {
        musec += 1000000;
        sec--;
    }
    printf(" [sec = %ld, musec = %ld] ", sec, musec);
}





















/* -- Start sending a sequence of requests */

/*string reply1 = chan.send_request("hello");
cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

string reply2 = chan.send_request("data Joe Smith");
cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

string reply3 = chan.send_request("data Jane Smith");
cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

for(int i = 0; i < 100; i++) {
    string request_string("data TestPerson" + int2string(i));
    string reply_string = chan.send_request(request_string);
    cout << "reply to request " << i << ":" << reply_string << endl;;
}

string reply4 = chan.send_request("quit");
cout << "Reply to request 'quit' is '" << reply4 << endl; */
