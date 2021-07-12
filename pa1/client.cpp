/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;

int getMicroSec(timeval t0, timeval t1) {
	return (t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec - t0.tv_usec;
}


int main(int argc, char *argv[]){
	FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
	FIFORequestChannel* currChan = &chan;
	
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
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:cm:")) != -1) {
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
				break;
			case 'm':
				msel = true;
				m = atoi(optarg);
				break;

		}
	}
	
    // sending a non-sense message, you need to change this
    char buf [MAX_MESSAGE]; // 256

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);
	
	

	if (csel) {
		MESSAGE_TYPE newChanmsg = NEWCHANNEL_MSG;
		currChan->cwrite(&newChanmsg, sizeof(MESSAGE_TYPE));
		char chanName[30];
		currChan->cread(chanName, 30);
		string channelName(chanName);
		static FIFORequestChannel extraChannel (channelName, FIFORequestChannel::CLIENT_SIDE); //static keeps it from going out of scope
		currChan = &extraChannel;
	}

    if (psel && !(esel || fsel || tsel)) {
		ofstream fs("x1.csv");
		int person = p;
		int ecgnum = 1;
		for (int i = 0; i < 1000; i++) {
			double timept = 0.004*i;
			datamsg ptmsg1 (person, timept, 1);
			datamsg ptmsg2 (person, timept, 2);
			double pt1;
			double pt2;
			currChan->cwrite(&ptmsg1, sizeof (datamsg));
			currChan->cread(&pt1, sizeof(double));
			currChan->cwrite(&ptmsg2, sizeof (datamsg));
			currChan->cread(&pt2, sizeof(double));

			if (!fs.is_open()) {
				cout << "Something went wrong opening x1.csv" << endl;
				return 1;
			}
			fs << timept << "," << pt1 << "," << pt2 << endl;

			//cout << "(t, pt1, pt2): " << "(" << timept << "," << pt1 << "," << pt2 << ")" << endl;
		}	
		//gettimeofday(&t1, NULL);
    } else if (psel && tsel && esel) {
		datamsg x (p, t, e);
	
		currChan->cwrite (&x, sizeof (datamsg)); // question
		double reply;
		int nbytes = currChan->cread (&reply, sizeof(double)); //answer
		cout << "For person n" << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
	} else if (fsel) {
		filemsg fmsg(0,0);
		int bufLen = sizeof(filemsg) + filename.size()+1;
		char fbuffer[bufLen]; // fbuffer is our "packet." We will smash a filemsg and string into it.
		memcpy(fbuffer, &fmsg, sizeof(filemsg));
		strcpy(fbuffer + sizeof(filemsg), filename.c_str());
		currChan->cwrite(fbuffer, bufLen);
		cout << "buffer length: " << bufLen << endl;

		// computing number of packets needed to transfer file
		__int64_t fileSize;
		__int64_t actualSize = m;
		currChan->cread(&fileSize, sizeof(__int64_t));
		__int64_t packetNo = ceil(fileSize/static_cast<double>(actualSize));
		cout << "number of packets: " << packetNo << endl;

		ofstream ofs("received/" + filename, std::ofstream::binary);
		if (!ofs.is_open()) {
			cout << "oof there was a problem" << endl;
			return 1;
		}
		
		for (__int64_t i = 0; i < packetNo-1; i++) { // iterate through first n-1 packets
			char themsg[actualSize];
			filemsg packetmsg(i*(actualSize), actualSize);
			memcpy(fbuffer, &packetmsg, sizeof(filemsg));
			strcpy(fbuffer + sizeof(filemsg), filename.c_str());
			currChan->cwrite(fbuffer, bufLen);
			currChan->cread(themsg, actualSize);
			ofs.write(themsg, actualSize);
		}
		__int64_t theRest = fileSize - (packetNo-1)*actualSize;
		cout << "final transfer size: " << theRest << endl;
		cout << "file size: " << fileSize << endl;
		char finalMsg[theRest];
		filemsg finalPacket((packetNo-1)*actualSize, theRest);
		memcpy(fbuffer, &finalPacket, sizeof(filemsg));
		strcpy(fbuffer + sizeof(filemsg), filename.c_str());
		currChan->cwrite(fbuffer, bufLen);
		currChan->cread(finalMsg, theRest);
		ofs.write(finalMsg, theRest);
	}

	gettimeofday(&t1, NULL);
	cout << "Action took: " << getMicroSec(t0, t1) << " microseconds" << endl;
    
	
	

	filemsg fm (0,0);
	string fname = "teslkansdlkjflasjdf.dat";
	
	int len = sizeof (filemsg) + fname.size()+1;
	char buf2 [len];
	memcpy (buf2, &fm, sizeof (filemsg));
	strcpy (buf2 + sizeof (filemsg), fname.c_str());
	currChan->cwrite (buf2, len);  // I want the file length;
	
	
	// closing the channel    
    MESSAGE_TYPE close = QUIT_MSG;
	chan.cwrite (&close, sizeof(MESSAGE_TYPE));
    currChan->cwrite (&close, sizeof (MESSAGE_TYPE));
	
}
