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
	int p = -1;
	double t = -1.0;
	int e = -1;
	int m = 256;
	
	string filename = "";
	while ((opt = getopt(argc, argv, "m:p:t:e:f:c")) != -1) {
		switch (opt) {
			case 'm':
				m = atoi(optarg);
				break;
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
		}
	}
	
    // sending a non-sense message, you need to change this
    //char buf [MAX_MESSAGE]; // 256
	if (p !=-1){
		if (t==-1.0){
			if (e == -1){
				for (double i = 0; i < 1000; i++){
					t = 0.004 * i;
					datamsg x (p, t, 1);
					chan.cwrite (&x, sizeof (datamsg)); // question
					double reply;
					int nbytes = chan.cread (&reply, sizeof(double)); //answer
					cout << t << reply;
					datamsg x2 (p, t, 2);
					chan.cwrite (&x2, sizeof (datamsg)); // question
					nbytes = chan.cread (&reply, sizeof(double)); //answer
					cout << reply << endl;
				}
			}
			else {
				for(double i = 0; i < 1000; i++){
					t = 0.004 * i;
					datamsg x (p, t, e);
					chan.cwrite (&x, sizeof (datamsg)); // question
					double reply;
					int nbytes = chan.cread (&reply, sizeof(double)); //answer
					cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;	
				}
			}
			
		}
		else if (e == -1){
			datamsg x (p, t, 1);
			chan.cwrite (&x, sizeof (datamsg)); // question
			double reply;
			int nbytes = chan.cread (&reply, sizeof(double)); //answer
			cout << "For person " << p <<", at time " << t << ", the value of ecg "<< 1 <<" is " << reply << endl;
			datamsg x2 (p, t, 2);
			chan.cwrite (&x2, sizeof (datamsg)); // question
			nbytes = chan.cread (&reply, sizeof(double)); //answer
			cout << "For person " << p <<", at time " << t << ", the value of ecg "<< 2 <<" is " << reply << endl;
		}
		else {
			datamsg x (p, t, e);
			chan.cwrite (&x, sizeof (datamsg)); // question
			double reply;
			int nbytes = chan.cread (&reply, sizeof(double)); //answer
			cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
		}
	}
	if (filename != ""){
		ofstream outfile ("received/"+filename, fstream::binary);
		int len = sizeof (filemsg) + filename.size()+1;
		char buf2 [len];
		filemsg a (0,0);
		memcpy (buf2, &a, sizeof (filemsg));
		strcpy (buf2 + sizeof (filemsg), filename.c_str());
		chan.cwrite(buf2, len);
		__int64_t filelen;
		chan.cread (&filelen, sizeof(__int64_t));

		__int64_t times = ceil(filelen/static_cast<double>(m));
		for (int i = 0; i<times-1; i++ ){
			filemsg fm (m * i,m);	//first is position, second is amount of bytes
			memcpy (buf2, &fm, sizeof (filemsg));
			strcpy (buf2 + sizeof (filemsg), filename.c_str());
			chan.cwrite (buf2, len);  // sends question requesting data from file
			char databuffer [m];
			chan.cread (databuffer, m);
			outfile.write(databuffer,m);
		}
		__int64_t tempv = filelen-((times-1)*m);
		cout << tempv << endl;
		cout << m << endl;
		filemsg fm ((times-1)*m, tempv);	//first is position, second is amount of bytes
		memcpy (buf2, &fm, sizeof (filemsg));
		strcpy (buf2 + sizeof (filemsg), filename.c_str());
		chan.cwrite (buf2, len);  // sends question requesting data from file
		char databuffer [tempv];
		chan.cread (databuffer, tempv);
		outfile.write(databuffer,tempv);


		
		
	}
	
	//creating a channel

	// closing the channel    
    MESSAGE_TYPE M = QUIT_MSG;
    chan.cwrite (&M, sizeof (MESSAGE_TYPE));
}
