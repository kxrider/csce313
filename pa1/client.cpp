/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;


int main(int argc, char *argv[]){
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
	
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	bool psel = false;
	bool tsel = false;
	bool esel = false;
	bool fsel = false;
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:")) != -1) {
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
		}
	}
	
    // sending a non-sense message, you need to change this
    char buf [MAX_MESSAGE]; // 256
    if (psel && !(esel || fsel || tsel)) {
		ofstream fs("x1.csv");
		int person = p;
		int ecgnum = 1;
		struct timeval t0;
		struct timeval t1;
		gettimeofday(&t0, NULL);
		for (int i = 0; i < 1000; i++) {
			double timept = 0.004*i;
			datamsg ptmsg1 (person, timept, 1);
			datamsg ptmsg2 (person, timept, 2);
			double pt1;
			double pt2;
			chan.cwrite(&ptmsg1, sizeof (datamsg));
			chan.cread(&pt1, sizeof(double));
			chan.cwrite(&ptmsg2, sizeof (datamsg));
			chan.cread(&pt2, sizeof(double));

			if (!fs.is_open()) {
				cout << "Something went wrong opening x1.csv" << endl;
				return 1;
			}
			fs << timept << "," << pt1 << "," << pt2 << endl;

			//cout << "(t, pt1, pt2): " << "(" << timept << "," << pt1 << "," << pt2 << ")" << endl;
		}	
		gettimeofday(&t1, NULL);
		cout << "The transfer took " << (t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec - t0.tv_usec << " microseconds." << endl;
    } else if (psel && tsel && esel) {
		datamsg x (p, t, e);
	
		chan.cwrite (&x, sizeof (datamsg)); // question
		double reply;
		int nbytes = chan.cread (&reply, sizeof(double)); //answer
		cout << "For person n" << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
	} else if (fsel) {
		filemsg fmsg(0,0);
		int bufLen = sizeof(filemsg) + filename.size()+1;
		char fbuffer[bufLen]; // fbuffer is our "packet." We will smash a filemsg and string into it.
		memcpy(fbuffer, &fmsg, sizeof(filemsg));
		strcpy(fbuffer + sizeof(filemsg), filename.c_str());
		chan.cwrite(fbuffer, bufLen);

		// computing number of packets needed to transfer file
		__int64_t fileSize;
		int actualSize = MAX_MESSAGE-1;
		chan.cread(&fileSize, sizeof(__int64_t));
		__int64_t packetNo = ceil(fileSize/static_cast<double>(actualSize));

		ofstream ofs("received/" + filename);
		if (!ofs.is_open()) {
			cout << "oof there was a problem" << endl;
			return 1;
		}

		for (__int64_t i = 0; i < packetNo-1; i++) { // iterate through first n-1 packets
			char themsg[actualSize];
			filemsg packetmsg(i*(actualSize), actualSize);
			memcpy(fbuffer, &packetmsg, sizeof(filemsg));
			strcpy(fbuffer + sizeof(filemsg), filename.c_str());
			chan.cwrite(fbuffer, bufLen);
			chan.cread(themsg, actualSize);
			// cout << "themsg: " << themsg << endl;
			ofs << themsg;
		}
		int theRest = fileSize - (packetNo-1)*actualSize;
		char finalMsg[theRest];
		cout << "num: " << fileSize << endl;
		filemsg finalPacket((packetNo-1)*actualSize, theRest);
		memcpy(fbuffer, &finalPacket, sizeof(filemsg));
		strcpy(fbuffer + sizeof(filemsg), filename.c_str());
		chan.cwrite(fbuffer, bufLen);
		chan.cread(finalMsg, theRest);
		cout << finalMsg;
		ofs << finalMsg;
	}
    
	
	

	filemsg fm (0,0);
	string fname = "teslkansdlkjflasjdf.dat";
	
	int len = sizeof (filemsg) + fname.size()+1;
	char buf2 [len];
	memcpy (buf2, &fm, sizeof (filemsg));
	strcpy (buf2 + sizeof (filemsg), fname.c_str());
	// cout << "buf2: " << buf2[0] << endl;
	chan.cwrite (buf2, len);  // I want the file length;
	
	
	// closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
