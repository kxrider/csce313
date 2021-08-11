/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include "SHMreqchannel.h"
#include <sys/wait.h>

using namespace std;

int getMicroSec(timeval t0, timeval t1) {
	return (t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec - t0.tv_usec;
}


int main(int argc, char *argv[]){
	
	
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	int m = MAX_MESSAGE;
	bool psel = false;
	bool tsel = false;
	bool esel = false;
	bool fsel = false;
	bool csel = false;
	bool msel = false;
	string ipcmethod = "f";
	int nchannels = 1;

	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:c:m:i:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				psel = true;
				break;
			case 't':
				t = atof (optarg);
				tsel = true;
				break;
			case 'e':
				e = atoi (optarg);
				esel = true;
				break;
			case 'f':
				filename = optarg;
				fsel = true;
				break;
			case 'c':
				csel = true;
				nchannels = atoi(optarg);
				break;
			case 'm':
				msel = true;
				m = atoi(optarg);
				break;
			case 'i':
				ipcmethod = optarg;
				std::cout << "ipc method changed to " << ipcmethod << endl;
				break;

		}
	}
	
	int server_pid = fork();
	if (server_pid) {
		string bufcap = to_string(m);
		execlp("./server", "./server", "-i", ipcmethod.c_str(), "-m", bufcap.c_str(), (char*) NULL);
	}

	RequestChannel* chan;
	if (ipcmethod == "f") {
		chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
	} else if (ipcmethod == "q") {
		chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE);
	} else if (ipcmethod == "m") {
		chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE, m);
	}
	RequestChannel* currChan = chan;

    // sending a non-sense message, you need to change this
    char buf [MAX_MESSAGE]; // 256

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);
	
	
	vector<RequestChannel*> channelVec {chan};
	vector<int> processVec;

	if (csel) {
		MESSAGE_TYPE newChanmsg = NEWCHANNEL_MSG;
		
		for (int i = 0; i < nchannels; i++) {
			currChan->cwrite(&newChanmsg, sizeof(MESSAGE_TYPE));
			char chanName[30];
			currChan->cread(chanName, 30);
			string channelName(chanName);

			RequestChannel* newChan;
			if (ipcmethod == "f") {
				newChan = new FIFORequestChannel(channelName, FIFORequestChannel::CLIENT_SIDE);
			} else if (ipcmethod == "q") {
				newChan = new MQRequestChannel(channelName, RequestChannel::CLIENT_SIDE);
			} else if (ipcmethod == "m") {
				newChan = new SHMRequestChannel(channelName, RequestChannel::CLIENT_SIDE, m);
			}
			channelVec.push_back(newChan);
		}
		currChan = channelVec.at(0);
	}

    if (psel && esel && !(fsel || tsel)) {
		//cout << "hm" << endl;
		int person = p;
		int ecgnum = 1;
		for (int i = 0; i < nchannels; i++) {
			int fork_id = fork();
			
			if (!fork_id) {
				ofstream fs("x" + to_string(i) + ".csv");
				//cout << "h" << endl;
				for (int j = 0; j < 1000; j++) {
					double timept = 0.004*j;
					datamsg ptmsg1 (person, timept, 1);
					//datamsg ptmsg2 (person, timept, 2);
					double pt1;
					//double pt2;
					channelVec.at(i)->cwrite(&ptmsg1, sizeof (datamsg));
					
					channelVec.at(i)->cread(&pt1, sizeof(double));
					//cout << "a" << endl;
					//currChan->cwrite(&ptmsg2, sizeof (datamsg));
					//currChan->cread(&pt2, sizeof(double));
					if (!fs.is_open()) {
						EXITONERROR("couldn't open file.");
					}
					//cout << timept << "," << pt1 << endl;
					fs << timept << "," << pt1 << endl;
				}
				return 0;
			//cout << "(t, pt1, pt2): " << "(" << timept << "," << pt1 << "," << pt2 << ")" << endl;
			} else {
				processVec.push_back(fork_id);
			}
		}
			
		//gettimeofday(&t1, NULL);
    } else if (psel && tsel && esel) {
		datamsg x (p, t, e);
		//cout << "writing stuff..." << endl;
		for (int i = 0; i < nchannels; i++) {
			channelVec.at(i)->cwrite (&x, sizeof (datamsg)); // question
			//cout << "done writing stuff..." << endl;
			double reply;
			int nbytes = channelVec.at(i)->cread (&reply, sizeof(double)); //answer
			std::cout << "Channel " << channelVec.at(i)->name() << ": For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
		}
		
	} else if (fsel) {
		filemsg fmsg(0,0);
		int bufLen = sizeof(filemsg) + filename.size()+1;
		char fbuffer[bufLen]; // fbuffer is our "packet." We will smash a filemsg and string into it.
		memcpy(fbuffer, &fmsg, sizeof(filemsg));
		strcpy(fbuffer + sizeof(filemsg), filename.c_str());
		currChan->cwrite(fbuffer, bufLen);
		//cout << "buffer length: " << bufLen << endl;

		// computing number of packets needed to transfer file
		__int64_t fileSize;
		__int64_t actualSize = m;
		currChan->cread(&fileSize, sizeof(__int64_t));
		__int64_t packetNo = ceil(fileSize/static_cast<double>(actualSize));
		std::cout << "number of packets: " << packetNo << endl;
		__int64_t packetPerChan = packetNo/nchannels;
		std::cout << "packets per channel " << packetPerChan << endl;

		for (__int64_t i = 0; i < nchannels; i++) {
			close(open(("received/" + filename).c_str(), O_CREAT | O_WRONLY, 0777));
			int forkid = fork();
			if (!forkid) {
				int fd = open(("received/" + filename).c_str(), O_WRONLY, 0777);
				if (fd < 0) {
					EXITONERROR("couldn't open file.");
				}
				lseek(fd, actualSize*packetPerChan*i, 0);
				if (i == nchannels - 1) {
					__int64_t remainingPackets = packetNo - (nchannels-1)*packetPerChan;
					for (__int64_t j = 0; j < remainingPackets-1; j++) {
						char themsg[actualSize];
						filemsg packetmsg((i*packetPerChan + j)*actualSize, actualSize);
						memcpy(fbuffer, &packetmsg, sizeof(filemsg));
						strcpy(fbuffer + sizeof(filemsg), filename.c_str());
						channelVec.at(i)->cwrite(fbuffer, bufLen);
						channelVec.at(i)->cread(themsg, actualSize);
						int bytesWritten;
						if (bytesWritten = write(fd, themsg, actualSize) < 0) {
							EXITONERROR("write error");
						}
					}

					__int64_t theRest = fileSize - (packetNo-1)*actualSize;
					std::cout << "final transfer size: " << theRest << endl;
					std::cout << "file size: " << fileSize << endl;
					char finalMsg[theRest];
					filemsg finalPacket((packetNo-1)*actualSize, theRest);
					memcpy(fbuffer, &finalPacket, sizeof(filemsg));
					strcpy(fbuffer + sizeof(filemsg), filename.c_str());
					channelVec.at(i)->cwrite(fbuffer, bufLen);
					channelVec.at(i)->cread(finalMsg, theRest);
					int bytesWritten;
					if (bytesWritten = write(fd, finalMsg, theRest) < 0) {
						EXITONERROR("write error");
					}
					return 0;
				}
				for (__int64_t j = 0; j < packetPerChan; j++) { // iterate through first n-1 packets
					char themsg[actualSize];
					filemsg packetmsg((i*packetPerChan + j)*actualSize, actualSize);
					memcpy(fbuffer, &packetmsg, sizeof(filemsg));
					strcpy(fbuffer + sizeof(filemsg), filename.c_str());
					channelVec.at(i)->cwrite(fbuffer, bufLen);
					channelVec.at(i)->cread(themsg, actualSize);
					int bytesWritten;
					if (bytesWritten = write(fd, themsg, actualSize) < 0) {
						EXITONERROR("write error");
					}
				}
				//int code = close(fd);
				return 0;
			} else {
				processVec.push_back(forkid);
			}
		}
	}

	while (processVec.size() > 0) {
		waitpid(processVec.at(0), 0, 0);
		//cout << "reaped  " << processVec.at(0) << endl;
		processVec.erase(processVec.begin());
	}

	gettimeofday(&t1, NULL);
	std::cout << "Action took: " << getMicroSec(t0, t1) << " microseconds" << endl;
	
	// closing the channel    
    MESSAGE_TYPE close = QUIT_MSG;
	//chan->cwrite (&close, sizeof(MESSAGE_TYPE));

	for (int i = 0; i < nchannels; i++) {
		channelVec.at(i)->cwrite(&close, sizeof(MESSAGE_TYPE));
	}
	
	for (int i = 0; i < nchannels; i++) {
		delete channelVec.at(i);
		channelVec.at(i) = nullptr;
	}

	waitpid(server_pid, 0, 0);
}
