/*
Code by
Govinda Rohith Y
CS21BTECH11062
Operating System-2 (OS-3523)
Programming Assignment 4
The Jurassic Park Problem
*/
#include <iostream>
#include <stdlib.h>
#include <random>
#include <pthread.h>
#include <signal.h>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <semaphore.h>
using namespace std;
class Dat // Data Structure to store request_time,entry_time,exit_time and carno
{
public:
    string request_time, entry_time, exit_time;
    int carno;
};
int p, c, l_p, l_c, k;      // No of passengers,No of cars,Lambda_P,Lamba_K
int *is_avail_car;          // true=> car is available
string *entry_mu, *exit_mu; // Array to store entry and exit times into the museum
Dat **req_dat;              // pxk
stringstream stream;
random_device rd;
mt19937 gen(rd());
sem_t sem_c, sem_bin;
// sempahores first one intiliased to number of cars and second one is used to protect *is_avail_car
sem_t *sem_ex_p, *sem_ex_c; // array of binary semaphores associated with each passenger and car thread
pthread_t *pid;             // to store thread id's of all threads
exponential_distribution<double> distribution1, distribution2;
// variables to generate exponentially distrubuted random number with given mean
exponential_distribution<double> var_rand(int meaner)
{
    // function to return objects which are used
    // to generate random exponentially distributed numbers with given mean
    double exp_dist_lambda = (double)1 / meaner;
    exponential_distribution<double> distribution(exp_dist_lambda);
    return distribution;
}
std::string get_time() // function to return timestamp in string format
{
    using namespace std::chrono;
    using clock = system_clock;

    const auto current_time_point{clock::now()};
    const auto current_time{clock::to_time_t(current_time_point)};
    const auto current_localtime{*std::localtime(&current_time)};
    const auto current_time_since_epoch{current_time_point.time_since_epoch()};
    const auto current_milliseconds{duration_cast<milliseconds>(current_time_since_epoch).count() % 1000};

    std::ostringstream stream;
    stream << std::put_time(&current_localtime, "%T") << "." << std::setw(3) << std::setfill('0') << current_milliseconds;
    return stream.str();
}
void sleeper(double req_time) // function to make sleep a thread for given time
{
    req_time = (double)req_time / 1000;
    struct timespec tim;
    tim.tv_sec = (int)req_time / 1;
    tim.tv_nsec = (req_time - (int)req_time / 1) * 1000000000;
    tim.tv_nsec = ceil(tim.tv_nsec);
    tim.tv_sec = ceil(tim.tv_sec);
    nanosleep(&tim, NULL);
}
void file_printer() // tbd
{
    int i, j;
    ofstream fp("output.txt");
    // Passenger 1 enters the museum at 12:00
    // Passenger 2 enters the museum at 12:01
    // Passenger 1 made a ride request at 12:01
    // Car 1 accepts passenger 1’s request
    // Passenger 1 started riding at 12:02
    // Car 1 is riding Passenger 1
    // Car 1 has finished Passenger 1’s tour
    // Passenger 1 finished riding at 12:04
    // Passenger 1 exit the museum at 12:10
    for (i = 0; i < p; i++)
    {
        fp << "Passenger " << i + 1 << " enters the museum at " << entry_mu[i] << endl;
    }
    for (i = 0; i < p; i++)
    {
        for (j = 0; j < k; j++)
        {
            // fp<<"Passenger "<<i+1<<"    ;Iteration - "<<j+1<<endl;
            fp << "Passenger " << i + 1 << " made a ride request"
               << " at " << req_dat[i][j].request_time << endl;
            fp << "Car " << req_dat[i][j].carno + 1 << " accepts passenger " << i + 1 << "'s request" << endl;
            fp << "Passenger " << i + 1 << " started raiding"
               << " at " << req_dat[i][j].entry_time << endl;
            fp << "Car " << req_dat[i][j].carno + 1 << " is raiding Passenger " << i + 1 << endl;
            fp << "Car " << req_dat[i][j].carno + 1 << " has finished Passenger " << i + 1 << "'s tour" << endl;
            fp << "Passenger " << i + 1 << " finished riding"
               << " at " << req_dat[i][j].exit_time << endl;
        }
    }
    for (i = 0; i < p; i++)
    {
        fp << "Passenger " << i + 1 << " exit the museum at " << exit_mu[i] << endl;
    }
    fp.close();
}
void *each_pas(void *arg) // function to execute for each passenger thread
{
    int i, j;
    double temp;
    std::string tempo;
    int t_no = *((int *)arg); // get thread number
    free(arg);
    tempo = get_time();
    entry_mu[t_no] = tempo; // entry time into musuem
    sem_wait(&sem_ex_p[t_no]);
    for (i = 0; i < k; i++)
    {
        temp = distribution1(gen);
        sleeper(temp); // roaming time
        tempo = get_time();
        req_dat[t_no][i].request_time = tempo; // request time
        sem_wait(&sem_c);                      // request for a car
        sem_wait(&sem_bin);                    // take available car
        for (j = t_no % c; j < c; j = (j + 1) % c)
        {
            if (is_avail_car[j] == -1)
            {
                is_avail_car[j] = t_no;
                break;
            }
        }
        sem_post(&sem_bin); // release available car
        // cout<<"Entered into critical section by thread :  "<<t_no<<"    Iteration ::  "<<i<<"   Selected Car no ::   "<<j<<endl;
        tempo = get_time();
        req_dat[t_no][i].entry_time = tempo; // request time
        req_dat[t_no][i].carno = j;
        sem_post(&sem_ex_c[j]);    // signalling the car to start
        sem_wait(&sem_ex_p[t_no]); // make the passenger thread to wait untill ride completes
        // cout<<"Done roaming by passenger  "<<t_no<<"    For car "<<j<<endl;
        is_avail_car[j] = -1;
        sem_post(&sem_c); // release the car
        tempo = get_time();
        req_dat[t_no][i].exit_time = tempo; // exit-time
    }
    tempo = get_time();
    exit_mu[t_no] = tempo;
    return NULL;
}
void *each_car(void *arg) // function to execute for each car thread
{
    int i, j;
    double temp;
    int t_no = *((int *)arg); // get thread number
    free(arg);
    sem_wait(&sem_ex_c[t_no]);
    while (true)
    {
        sem_wait(&sem_ex_c[t_no]); // wait for passenger's arrival
        temp = distribution2(gen);
        // cout<<"Requesting raid by passenger "<<is_avail_car[t_no]<<"    For time "<<temp<<" And for car number "<<t_no<<endl;
        sleeper(temp);                           // car roaming time time
        sem_post(&sem_ex_p[is_avail_car[t_no]]); // signal passenger thread that "RAID IS DONE"
    }
    return 0;
}
void museum_car() // function to create required number of threads for execution
{
    int i, temp;
    pthread_t tid = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pid = (pthread_t *)calloc((p + c), (sizeof(pthread_t)));
    for (i = 0; i < c; i++)
    {
        sem_init(&sem_ex_c[i], 0, 1);
    }
    for (i = 0; i < p; i++)
    {
        sem_init(&sem_ex_p[i], 0, 1);
    }
    sem_init(&sem_c, 0, c);
    sem_init(&sem_bin, 0, 1);
    for (i = 0; i < p; i++)
    {
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i;
        // create p threads
        if (pthread_create(&tid, &attr, each_pas, arg) != 0)
        {
            printf("Error occured in creating the thread \n");
        }
        pid[i] = tid;
    }
    for (i = 0; i < c; i++)
    {
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i;
        // create c threads
        if (pthread_create(&tid, &attr, each_car, arg) != 0)
        {
            printf("Error occured in creating the thread \n");
        }
        pid[i + p] = tid;
    }
    for (i = 0; i < p; i++) // wait for passenger thread to join
        pthread_join(pid[i], NULL);
    file_printer();         // calling file printer to print output file
    for (i = 0; i < c; i++) // destroying existing semaphores
    {
        sem_destroy(&sem_ex_c[i]);
    }
    for (i = 0; i < p; i++)
    {
        sem_destroy(&sem_ex_p[i]);
    }
    sem_destroy(&sem_c);
    sem_destroy(&sem_bin);
    file_printer(); // calling file printer to print output file
}
void metric_p1() // Function to print data which is used for plotting plot 1
{
    int i;
    unsigned long long int avg = 0, worst = 0, hour = 0, minute = 0, sec = 0, msec = 0;
    unsigned long long int differ = 0;
    for (i = 0; i < p; i++)
    {
        hour = (stoi(exit_mu[i].substr(0, 2)) - stoi(entry_mu[i].substr(0, 2))) * 60 * 60 * 1000;
        minute = (stoi(exit_mu[i].substr(3, 2)) - stoi(entry_mu[i].substr(3, 2))) * 60 * 1000;
        sec = (stoi(exit_mu[i].substr(6, 2)) - stoi(entry_mu[i].substr(6, 2))) * 1000;
        msec = (stoi(exit_mu[i].substr(9, 2)) - stoi(entry_mu[i].substr(9, 2)));
        differ = hour + minute + sec + msec;
        avg += differ;
    }
    cout << "Average time taken by passenger " << (double)avg / p << " milliseconds" << endl;
}
void metric_p2() // Function to print data which is used for plotting plot 2
{
    int i, j;
    unsigned long long int hour = 0, minute = 0, sec = 0, msec = 0;
    unsigned long long int differ = 0, *store;
    store = (unsigned long long int *)calloc(c, sizeof(unsigned long long int));
    for (i = 0; i < p; i++)
    {
        for (j = 0; j < k; j++)
        {
            hour = (stoi(req_dat[i][j].exit_time.substr(0, 2)) - stoi(req_dat[i][j].entry_time.substr(0, 2))) * 60 * 60 * 1000;
            minute = (stoi(req_dat[i][j].exit_time.substr(3, 2)) - stoi(req_dat[i][j].entry_time.substr(3, 2))) * 60 * 1000;
            sec = (stoi(req_dat[i][j].exit_time.substr(6, 2)) - stoi(req_dat[i][j].entry_time.substr(6, 2))) * 1000;
            msec = (stoi(req_dat[i][j].exit_time.substr(9, 2)) - stoi(req_dat[i][j].entry_time.substr(9, 2)));
            differ = hour + minute + sec + msec;
            store[req_dat[i][j].carno] += differ;
        }
    }
    differ = 0;
    for (i = 0; i < c; i++)
    {
        differ = differ + store[i];
    }
    cout << "Average time taken by the cars to complete its tour " << (double)differ / (c) << " milliseconds" << endl;
    free(store);
}
int main()
{
    int i;
    FILE *fp;
    fp = fopen("inp-params.txt", "r");
    if (fscanf(fp, "%d %d %d %d %d", &p, &c, &l_p, &l_c, &k) == EOF)
    {
        cout << "Error reading file" << endl;
        fclose(fp);
        return 0;
    }
    if (p <= 0 || c <= 0 || l_p <= 0 || l_c <= 0 || k <= 0)
    {
        cout << "Invalid inputs" << endl;
        fclose(fp);
        return 0;
    }
    fclose(fp);
    distribution1 = var_rand(l_p);
    distribution2 = var_rand(l_c);
    sem_ex_c = (sem_t *)malloc(sizeof(sem_t) * c);
    sem_ex_p = (sem_t *)malloc(sizeof(sem_t) * p);
    is_avail_car = (int *)malloc(c * sizeof(int));
    for (i = 0; i < c; i++)
        is_avail_car[i] = -1;
    entry_mu = (string *)calloc(p, sizeof(string));
    exit_mu = (string *)calloc(p, sizeof(string));
    req_dat = (Dat **)calloc(p, sizeof(Dat *));
    for (i = 0; i < p; i++)
    {
        req_dat[i] = (Dat *)calloc(k, sizeof(Dat));
    }
    museum_car(); // main function for solution
    cout << "*** Statistical Data ***" << endl;
    metric_p1();
    metric_p2();
    // free memory section
    free(entry_mu);
    free(exit_mu);
    for (i = 0; i < p; i++)
    {
        free(req_dat[i]);
    }
    free(req_dat);
    for (i = p; i < p + c; i++) // killing all the car threads to prevent infinite looping
        pthread_kill(pid[i], SIGKILL);
    free(pid);
    return 0;
}