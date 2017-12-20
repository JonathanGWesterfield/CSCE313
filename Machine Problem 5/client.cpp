/*
    File: client.cpp

    Author: Ya boi its me. Jabroni.
            Department of Computer Science
            Texas A&M University
    Date  : Does it matter?

    Client main program for MP3 in CSCE 313
*/

#include <stdlib.h>
#include <iomanip>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

#include "NetworkRequestChannel.H""
#include "BoundedBuffer.h"
#include "FileHandler.h"

/*--------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES */
/*--------------------------------------------------------------------------*/

void* requestData(void* paramID);
void* eventHandler(void* c);
void* statisticsThread(void* person_id);
void printHistogram(vector<int> data, string name, int personID);
void errorMess(char *msg);
void print_time_diff(struct timeval * tp1, struct timeval * tp2);


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
int nRequests = 10000;
int wThreads = 30;
int buff_size = 500;
string HostName = "localHost";
int port = 10250;

/* request count - counts the number of times that person has been requested */
int joeCounter = 0;
int janeCounter = 0;
int johnCounter = 0;

vector<int> joeHist(100); // histogram for each person
vector<int> janeHist(100);// 100 spots for each possible number
vector<int> johnHist(100);

BoundedBuffer* requestBuffer; // basic bounded buffer to hold requests
BoundedBuffer* joeBuff; //statistics buffers
BoundedBuffer* janeBuff;//delay new operator until complete program arguments
BoundedBuffer* johnBuff;

vector<NetworkRequestChannel*> NetworkChannels;
int* person_id;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

const int * joeID = new int(0); // Number ID's for Joe, Jane and John
const int * janeID = new int(1);
const int * johnID = new int(2);

/*--------------------------------------------------------------------------*/
/* M A I N   F U N C T I O N */
/*--------------------------------------------------------------------------*/

/**
 * @brief main function. It takes in the program arguements and stores them
 *      into the appropriate global variables. From there, it starts up the
 *      requestData, WorkerThread and statisticsThread in the typical
 *      concurrent way. It then calls a print function to output the histogram
 *      and destroys any leftover network Channels
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char * argv[])
{
    string temp;
    int c = 0;
    while((c = getopt (argc, argv, "n:w:b:p:n:")) != -1 )
        switch(c)
        {
            case 'n':
                nRequests = atoi(optarg);
                break;
            case 'w':
                wThreads = atoi(optarg);
                break;
            case 'b':
                buff_size = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                temp = optarg;
                if(temp != "")
                    HostName = temp;
                break;
            case '?':
                printf("Unknown Option\nAborting!!!\n");
                abort();
            default:
                printf("cmd line error\n");
                printf("options -n=NumRequests -w=NumWorkerThreads -b=BufferSize -p=PortNo -h=ServerHostName\n\n");
                abort();
        }
    pthread_t requestThreads[3];// one for each person 0=joe 1=jane 2=john
    pthread_t eventHandlerThread;
    pthread_t statThreads[3];

    requestBuffer = new BoundedBuffer(buff_size);
    joeBuff = new BoundedBuffer(buff_size);
    janeBuff = new BoundedBuffer(buff_size);
    johnBuff = new BoundedBuffer(buff_size);

    printf("CLIENT STARTED: \n");

    struct timeval tp_start; /* Used to compute elapsed time. */
    struct timeval tp_end;

    assert(gettimeofday(&tp_start, 0) == 0);

    // creates and runs the request threads
    if(pthread_create(&requestThreads[0], NULL, requestData, (void*)joeID) == -1) // thread, NULL, function, function args
        errorMess((char*)"Can't create thread for Joe. Fuck Joe");
    // fprintf(stderr, "%s: %s\n", "Can't create thread for Joe. Fuck Joe", strerrorMess(errno));
    if(pthread_create(&requestThreads[1], NULL, requestData, (void*)janeID) == -1)
        errorMess((char*)"Can't create thread for Jane. Fuck Jane");
    // fprintf(stderr, "%s: %s\n", "Can't create thread for Jane. Fuck Jane", strerror(errno));
    if(pthread_create(&requestThreads[2], NULL, requestData, (void*)johnID) == -1)
        errorMess((char*)"Can't create thread for John. Fuck John");
    // fprintf(stderr, "%s: %s\n", "Can't create thread for John. Fuck John", strerror(errno));

    printf("Creating event Handler\n");
    pthread_create(&eventHandlerThread, NULL, eventHandler, NULL);

    /** Creates all of our statistics threads */
    printf("Creating stat threads\n");
    if(pthread_create(&statThreads[0], NULL, statisticsThread, (void*)joeID) == -1) // thread, NULL, function, function args
        errorMess((char*)"Can't create statistics thread for Joe. Fuck Joe");
    //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for Joe. Fuck Joe", strerror(errno));
    if(pthread_create(&statThreads[1], NULL, statisticsThread, (void*)janeID) == -1)
        errorMess((char*)"Can't create statistics thread for Jane. Fuck Jane");
    //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for Jane. Fuck Jane", strerror(errno));
    if(pthread_create(&statThreads[2], NULL, statisticsThread, (void*)johnID) == -1)
        errorMess((char*)"Can't create statistics thread for John. Fuck John");
    //fprintf(stderr, "%s: %s\n", "Can't create statistics thread for John. Fuck John", strerror(errno));

    /** Join all of the request threads with error messages just in case */
    if (pthread_join(requestThreads[0], NULL) == -1)
        errorMess((char*)"Can't join 1st request thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 1st request thread", strerror(errno));
    if (pthread_join(requestThreads[1], NULL) == -1)
        errorMess((char*)"Can't join 2nd request thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 2nd request thread", strerror(errno));
    if (pthread_join(requestThreads[2], NULL) == -1)
        errorMess((char*)"Can't join 3rd request thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 3rd request thread", strerror(errno));

    pthread_join(eventHandlerThread, NULL); //wait for event handler to finish

    printf("Got back to main and joined the event handler Safe and Sound\n");

    /** Join all of the worker threads with error messages just in case */
    if (pthread_join(statThreads[0], NULL) == -1)
        errorMess((char*)"Can't join 1st statistics thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 1st statistics thread", strerror(errno));
    if (pthread_join(statThreads[1], NULL) == -1)
        errorMess((char*)"Can't join 2nd statistics thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 2nd statistics thread", strerror(errno));
    if (pthread_join(statThreads[2], NULL) == -1)
        errorMess((char*)"Can't join 3rd statistics thread");
    //fprintf(stderr, "%s: %s\n", "Can't join 3rd statistics thread", strerror(errno));

    assert(gettimeofday(&tp_end, 0) == 0); // gets the end timer

    sleep(1); //wait for server prints to close

    /** print out the histograms for each person */
    printHistogram(joeHist, "Joe Smith", *joeID);
    printHistogram(janeHist, "Jane Smith", *janeID);
    printHistogram(johnHist, "John Smith", *johnID);

    printf("Total number of requests: %d\n", nRequests * 3);
    printf("Total number of worker threads: %d\n", wThreads);
    print_time_diff(&tp_start, &tp_end);
    printf("\n");

    //Clean Up
    for (int i = 0;i < NetworkChannels.size(); i++)
        delete NetworkChannels[i];
    return 0;
}

/*--------------------------------------------------------------------------*/
/* THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

/**
 * @brief inserts the name of each person into the request buffer
 * @param paramID
 * @return
 */
void* requestData(void* paramID)
{
    // personID specifies what name to put into the buffer
    int personID = *((int*)paramID); // gets the ID and turns it to an int

    /** joe, jane, john are global string variables */
    for(int i = 0; i < nRequests; i++)
    {
        if(personID == 0)
        {
            printf("Inserting %s %d Into Request Buffer\n", "Joe", personID);
            requestBuffer->AddToBuffer(Response("data Joe Smith", personID, joeCounter)); // sends "data Joe"
        }
        else if(personID == 1)
        {
            printf("Inserting %s %d Into Request Buffer\n", "Jane", personID);
            requestBuffer->AddToBuffer(Response("data Jane Smith", personID, janeCounter)); // sends "data Jane"
        }
        else if(personID == 2)
        {
            printf("Inserting %s %d Into Request Buffer\n", "Jane", personID);
            requestBuffer->AddToBuffer(Response("data John Smith", personID, johnCounter)); // sends John
        }
    }
    printf("REQUEST THREAD IS NOW FINISHED\n\n");
}

/**
 * @brief Communicates with the server across the network. Pulls the data out of the bounded buffer and
 * puts the returned data into the statistics buffer for each person.
 * @param c
 * @return
 */
void* eventHandler(void* c)
{
    person_id=new int[wThreads];

    fd_set readset; //read file descriptors
    int max = 0;
    int select_result;

    Response response = Response("", 0,0);
    bool finished = false;
    int wcount = 0;
    int rcount = 0;
    struct timeval te = {0,10};

    for(int i = 0; i < wThreads; i++)
    {
        NetworkRequestChannel* channel = new NetworkRequestChannel(HostName, port);
        NetworkChannels.push_back(channel);
        person_id[i] = -1;//initialize to a - number so that it can not be mistaken for a real request
    }


    // fill all the channels with requests before trying to read from them
    for(int i = 0; i < wThreads; i++)
    {
        response = requestBuffer->RemoveFromBuffer();
        wcount++;
        NetworkChannels[i]->cWrite(response.requestData);
        person_id[i] = response.requestID;
    }

    while(!finished)
    {
        FD_ZERO(&readset);
        for(int i = 0; i < wThreads; i++)
        {
            if(NetworkChannels[i]->readFileDesc() > max)
            {
                max = NetworkChannels[i]->readFileDesc();
            }
            FD_SET(NetworkChannels[i]->readFileDesc(), &readset);
        }

        select_result = select(max+1, &readset, NULL, NULL, &te);

        if(select_result)
        {
            for(int i = 0; i < wThreads; i++)
            {
                if(FD_ISSET(NetworkChannels[i]->readFileDesc(), &readset))
                {
                    printf("Reading\n");
                    string serv_resp = NetworkChannels[i]->cRead();
                    rcount++;

                    switch(person_id[i])
                    {
                        case 0:
                            joeBuff->AddToBuffer(Response(serv_resp, 0, 0));
                            break;
                        case 1:
                            janeBuff->AddToBuffer(Response(serv_resp, 1, 0));
                            break;
                        case 2:
                            johnBuff->AddToBuffer(Response(serv_resp, 2, 0));
                            break;
                    }

                    if(wcount < nRequests*3)//dont write more than is available might cause deadlock
                    {
                        response = requestBuffer->RemoveFromBuffer();
                        wcount++;
                        // cout << "writing\n";
                        NetworkChannels[i]->cWrite(response.requestData);
                        person_id[i] = response.requestID;
                    }
                }
            }
        }

        if(rcount == nRequests * 3) //if all reads complete break
        {
            break;
        }
    }

    // close request channels
    for(int i = 0; i < wThreads; i++)
    {
        NetworkChannels[i]->cWrite("quit");
    }
}

/**
 * @brief Takes the response from the appropriate buffer (defined by the
 *      buffer ID that is passed in. It goes to the appropriate histogram
 *      vector, and then increase the value at the index (specified by the
 *      server response) and increments it by 1
 * @param bufferID
 * @return
 */
void* statisticsThread(void* person_id)
{
    int personID = *((int*)person_id);

    Response response("dummy", -1, -1);

    for(int i = 0; i < nRequests; i++)
    {
        if (personID == 0)
        {
            response = joeBuff->RemoveFromBuffer();
            joeHist[atoi(response.requestData.c_str())]++; //increment corresponding integer of the histogram
        }
        else if (personID ==1)
        {
            response = janeBuff->RemoveFromBuffer();
            janeHist[atoi(response.requestData.c_str())]++;
        }
        else if (personID == 2)
        {
            response = johnBuff->RemoveFromBuffer();
            johnHist[atoi(response.requestData.c_str())]++;
        }
    }
    printf("Stat Thread has finished!!!: %d\n", personID);
}


/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/


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

/**
 * @brief Generic error message
 * @param msg
 */
void errorMess(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}