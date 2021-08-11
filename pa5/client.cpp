#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "TCPreqchannel.h"
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

/*
int currFile = 0;
__int64_t totalLen;
bool done = false;

void patientPrintThread(HistogramCollection* hc) {
    while (!done) {
        sleep(2);
        system("clear");
        hc->print();
    }
}

void filePrintThread() {
    while (!done) {
        sleep(2);
        system("clear");
        cout << "File transfer progress: " << currFile/totalLen << "%" << endl;
    }
}*/

void patient_thread_function(int pno, int n, BoundedBuffer* reqb){
    datamsg d(pno, 0.0, 1);
    for (int i = 0; i < n; i++) {
        vector<char> temp((char*)&d, (char*)&d + sizeof(d));
        reqb->push(temp);
        d.seconds += 0.004;
    }
}

void file_thread_function(string fileName, int m, TCPRequestChannel* control, BoundedBuffer* reqBuffer) {
    
    string fn = "recv/" + fileName;
    FILE* fp = fopen(fn.c_str(), "w");
    if (fp == NULL) EXITONERROR("oof big bad");
    fclose(fp);

    int filemsgLen = sizeof(filemsg) + fileName.size() + 1;
    char filemsgBuf[filemsgLen];

    filemsg* fm = new (filemsgBuf) filemsg(0, 0);
    strcpy(filemsgBuf + sizeof(filemsg), fileName.c_str());
    
    control->cwrite(filemsgBuf, sizeof(filemsgBuf));

    __int64_t fileLen;
    //totalLen = fileLen;
    
    control->cread(&fileLen, sizeof(fileLen));

    while (fileLen) {
        //cout << "howdy" << endl;
        fm->length = min((__int64_t) m, fileLen);
        vector<char> temp (filemsgBuf, filemsgBuf + sizeof(filemsgBuf));
        reqBuffer->push(temp);
        fileLen -= fm->length;
        fm->offset += fm->length;
    }

}

struct Response {
    int pno;
    double ecg;
};

//mutex workerMut;

void worker_thread_function(BoundedBuffer* reqb, TCPRequestChannel* chan, BoundedBuffer* resb, int bufcap){
    char response[bufcap];
    while (true) {
        vector<char> req = reqb->pop();
        char* request = req.data();
        chan->cwrite(request, req.size());

        MESSAGE_TYPE* m = (MESSAGE_TYPE*) request;
        if (*m == DATA_MSG) {
            chan->cread(response, sizeof(double));
            int pno= ((datamsg*) request)->person;
            double ecg = *(double *) response;
            Response r {pno, ecg};
            vector<char> temp ((char*)&r, (char*)&r + sizeof(r)); // come back to this
            resb->push(temp);
        } else if (*m == QUIT_MSG) {
            break;
        } else if (*m == FILE_MSG) {
            //cout << "getting here" << endl;
            chan->cread(response, sizeof(response));
            filemsg* fm = (filemsg*) request;
            string fname = (char*) (fm + 1);
            fname = "recv/" + fname;
            FILE* fp = fopen(fname.c_str(), "r+");
            fseek(fp, fm->offset, SEEK_SET);
            fwrite(response, 1, fm->length, fp);
            fclose(fp);
            //workerMut.lock();
            //currFile += fm->length;
            //workerMut.unlock();
        }
    }
}
void histogram_thread_function (BoundedBuffer* resb, HistogramCollection* hc){
    while (true) {
        vector<char> response = resb->pop();
        Response* r = (Response*) response.data();
        if (r->pno == -1)
            break;
        hc->update(r->pno, r->ecg);
    }
}



int main(int argc, char *argv[])
{
    int n = 100;    //default number of requests per "patient"
    int p = 10;     // number of patients [1,15]
    int w = 100;    //default number of worker threads
    int b = 20; 	// default capacity of the request buffer, you should change this default
	int h = 5;
    int m = MAX_MESSAGE; 	// default capacity of the message buffer
    srand(time_t(NULL));
    string fileName = "";
    int c = -1;
    string host, port;

    while ((c = getopt(argc, argv, "p:m:f:b:w:n:h:r:")) != -1) {
        switch (c) {
            case 'p':
                p = atoi(optarg);
                break;
            case 'm':
                m = atoi(optarg);
                break;
            case 'f':
                fileName = optarg;
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'h':
                //h = atoi(optarg);
                host = optarg;
                break;
            case 'r':
                port = optarg;
                break;     
        }
    }
    
    BoundedBuffer request_buffer(b);
    BoundedBuffer response_buffer(b);
	HistogramCollection hc;
	
    

    struct timeval start, end;
    gettimeofday (&start, 0);

    // creating channel for each worker
    TCPRequestChannel* wchans[w];
    for (int i = 0; i < w; i++) {
        wchans[i] = new TCPRequestChannel(host, port);
    }
    
    thread histthreads[5];
    thread patients[p];
    thread workers[w];
    thread filethread;
    thread fileprint;
    thread patientprint;


    for (int i = 0; i < w; i++) {
        workers[i] = thread (worker_thread_function, &request_buffer, wchans[i], &response_buffer, m);
    }

    if (fileName.empty()) {
        for (int i = 0; i < p; i++) {
            Histogram* hist = new Histogram(10, -2.0, 2.0);
            hc.add(hist);
            patients[i] = thread(patient_thread_function, i+1, n, &request_buffer);
        }

        for (int i = 0; i < 5; i++) {
            histthreads[i] = thread(histogram_thread_function, &response_buffer, &hc);
        }
        //patientprint = thread(patientPrintThread, &hc);
        //patientprint.detach();
        for (int i = 0; i < p; i++) {
            //cout << "umm" << endl;
            patients[i].join();
        }
    } else {
        TCPRequestChannel tempChan(host, port);
        filethread = thread(file_thread_function, fileName, m, &tempChan, &request_buffer);
        //fileprint = thread(filePrintThread);
        //fileprint.detach();
        filethread.join();
        //done = true;
        
    }
    
    MESSAGE_TYPE q = QUIT_MSG;
    vector<char>  temp((char*)&q, (char*)&q + sizeof(q));
    for (int i = 0; i < w; i++) {
        request_buffer.push(temp);
    }

    for (int i = 0; i < w; i++) {
        workers[i].join();
    }

    if (fileName.empty()) {
        Response rq {-1, 0.0};
        vector<char> t ((char*)&rq,(char*)&rq + sizeof(rq) );
        for (int i = 0; i < 5; i++) {
            response_buffer.push(t);
        }

        for (int i = 0; i < 5; i++) {
            histthreads[i].join();
        }
    }
    //done = true;
    
    gettimeofday (&end, 0);
    // print the results
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec);///(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    std::cout << "Took " << secs + usecs << " micro seconds" << endl;

    if (fileName.empty())
        hc.print();
    //MESSAGE_TYPE q = QUIT_MSG;
    for (int i = 0; i < w; i++)
        delete wchans[i];
    
    //waitpid(pid, 0, 0);
    std::cout << "All Done!!!" << endl;
    
}
