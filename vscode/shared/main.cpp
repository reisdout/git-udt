/*****************************VISAO GERAL****************************************************
 ********************************************************************************************
 Os clientes enviam dados
 Os servidores recebem
 As classes extractors se voltam para a extração de features dos dumps.
 As classes savers se referem ao salvamento de features ao longo da troca de dados entre
 servidores e clientes.
 Caso interessante está na opção tcp_feature_saver, que traz a conbinação entre as classe
 class_feature_extractor_tcp_client e class_feature_saver_tcp. 
 A class_feature_extractor_tcp_client EXTRAI (por isso extractor) as features do dump 
 do arquivo levantado dos fluxos dos clientes e as passa para 
 class_feature_saver_tcp, que gera o arquivo da médias móveis associadas aos seq. 
 Essa tarefa de associação dos seq às médias móveis(ewma) seq é próprio das  classes savers. 
 Já associação dos seq a ocupação da fila é própria dos extractors, uma vez
 que depende de mineração do dump gerado no roteador.
 ********************************************************************************************
 ********************************************************************************************/




#include <libudt/udt.h>
#include <arpa/inet.h>
#include <cstring>
//#include <VegasMLP13/include/VegasMLP13.h>
#include <project_pure_udt/include/pure_udt.h>
#include <TCP_Socket/include/tcp_client.h>
#include <TCP_Socket/include/tcp_server.h>
#include <project_feature_saver/include/class_feature_saver_udt.h>
#include <project_feature_saver/include/class_feature_saver_tcp.h>
#include <project_feature_extractor/include/class_feature_extractor_udt.h>
#include <project_feature_extractor/include/class_feature_extractor_tcp.h>
#include <project_feature_extractor/include/class_feature_extractor_tcp_client.h>
#include <project_report/include/class_report.h>
#include "mrs_utils.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include<iomanip>
#include<typeinfo>

#include "defines/defines.h"



using namespace UDT;
using namespace std;
//using namespace std::chrono;



template <class T>
void main_print(string par_string, T par_var,bool par_pause=true) {

    
    float type_float;
    double type_double;
    long double type_long_double;
    

    const type_info& t_float = typeid(type_float);
    const type_info& t_double = typeid(type_double);  
    const type_info& t_long_double = typeid(type_long_double);
    const type_info& t_my_type = typeid(par_var);
    
    
    bool print = true;
    if(print)
    {

        //cout << "entering print " << endl;
        char c;
        if(t_my_type == t_float || t_my_type == t_double)
            cout << par_string << fixed << setprecision(5) << par_var << endl;
        else if (t_my_type == t_long_double)
            cout << par_string << fixed << setprecision(10) << par_var << endl;
        else
            cout << par_string << par_var << endl;
        cin >> c;
    }
}



int server_port = 0;

char filePath[] = "/home/ns/UDT-workspace/git-udt/vscode/shared/main.txt";

int send_type = PCC;

int delta_ack = 20;

bool send_lock = false;

int opt_pure_udt = -1;

u_int32_t last_ack;

string data_rate;
string num_flows;
string tipo_dado;
string simu_start_time;
string terminal_type; //cliente? servidor?, etc..
string congestion_control;
string experiment_path;


UDTSOCKET client;
UDTSOCKET serv;
UDTSOCKET recver;
//UDTSOCKET recent;//receptor do cliente

map<uint32_t, high_resolution_clock::time_point> map_seq_timestamp;

sockaddr_in serv_addr; //endereço do servidor, no cliente
int namelen_server_addr_in_client; //endereco do servidor capturado no cliente
sockaddr_in server_addr_in_client; //endereco ip do servidor capturado no cliente

sockaddr_in my_addr;//endereco do servidor, no servidor

//VegasMLP13* cchandle = NULL;
class_pure_udt* cchandle = NULL;

unsigned transmission_size = 170000;

void bone(char* par_buffer, int par_size)
{
    bzero(par_buffer,par_size);
    for(int i=0; i < par_size; i++)
    {
        par_buffer[i] = par_buffer[i]+1;
    }
}


void ConfigureClientOptions()
{
    int buffer = NET_BW*NET_RTT + 150000;
    UDT::setsockopt(client, 0, UDT_SNDBUF, &buffer, sizeof(int));
    UDT::setsockopt(client, 0, UDP_SNDBUF, &buffer, sizeof(int));
    UDT::setsockopt(client, 0, UDT_RCVBUF, &buffer, sizeof(int));
    UDT::setsockopt(client, 0, UDP_RCVBUF, &buffer, sizeof(int));  


}


void ConfigureServerOptions()
{
    int buffer = NET_BW*NET_RTT + 150000;
    UDT::setsockopt(recver, 0, UDT_RCVBUF, &buffer, sizeof(int));
    UDT::setsockopt(recver, 0, UDP_RCVBUF, &buffer, sizeof(int));
    UDT::setsockopt(recver, 0, UDT_SNDBUF, &buffer, sizeof(int));
    UDT::setsockopt(recver, 0, UDP_SNDBUF, &buffer, sizeof(int));
  

}

void Pause()
{
    char c;
    cin>>c;
}

void Pausar_por_tempo_aleatorio_micro_segundos(int par_tempo_minimo_micro_segundos)
{
    int random_number;    
    srand(time(0));
    random_number = rand()%10;
    usleep(par_tempo_minimo_micro_segundos +((random_number*100000)/10));

}

void ConfigureClientTransmissionRate()
{
    /*
    O tamanho é ajustado de forma que 
    a soma da velocidade total de todos os fluxos de 100Mbps
    levando-se em consideração envios a cada 0.1seg.
    */

    if(NUM_FLOWS == 20)
        transmission_size = 2 * MAX_PCC_SND_SIZE_20_FLUXOS;
    else if(NUM_FLOWS == 40)
        transmission_size = 2 * MAX_PCC_SND_SIZE_40_FLUXOS;
    else if(NUM_FLOWS == 60)
        transmission_size = 2 * MAX_PCC_SND_SIZE_60_FLUXOS;
    else if(NUM_FLOWS == 80)
        transmission_size = 2 * MAX_PCC_SND_SIZE_80_FLUXOS;
    else{
        cout << "Undefined transmission Size" << endl;
        exit(0);
    }        


}


void Set_client_socket()
{


    if(send_type == FILE || send_type == HELLO)
        client= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == PCC)
        client= UDT::socket(AF_INET, SOCK_DGRAM , 0);
    else if (send_type == MESSAGE )
        client= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    else
    {
        cout << "Tipo de envio invalido" << endl;
        exit(0);
    }
    
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_port = htons(9000);
    serv_addr.sin_port = htons(server_port);
    //inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    inet_pton(AF_INET, "10.0.1.3", &serv_addr.sin_addr);
    memset(&(serv_addr.sin_zero), '\0', 8);
    //int optResult = UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> )); 
    //std::cout << "optResult: " << optResult << std::endl; 
    opt_pure_udt = UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<class_pure_udt> , sizeof(CCCFactory<class_pure_udt> )); 
    std::cout << "opt_pure_udt: " << opt_pure_udt << std::endl;
    //Pause();
    

    
    // connect to the server, implict bind
    
    /*Aparentemente, quando do conect, há um bind(reserva) tácito na máquiana que 
    está solicitando a conexao*/
    while (UDT::ERROR == UDT::connect(client, (sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        std::cout << "connect: " << UDT::getlasterror().getErrorMessage();
        Pausar_por_tempo_aleatorio_micro_segundos(5000000);
        //exit(0);
    }
    //UDT::listen(client, 10);
    //recent = UDT::accept(client, (sockaddr*)&server_addr_in_client, &namelen_server_addr_in_client);
    ConfigureClientOptions();
    ConfigureClientTransmissionRate();
}

void Set_server_socket()
{
    if(send_type == FILE || send_type == HELLO)
        serv= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == PCC)
        serv= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    else if (send_type == MESSAGE )
        serv= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    else
    {
        cout << "Tipo de envio invalido" << endl;
        exit(0);
    }
    
    my_addr.sin_family = AF_INET;
    //my_addr.sin_port = htons(9000);
    my_addr.sin_port = htons(server_port);

    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);
    
    //int optResult = UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> )); 
    //std::cout << "optResult Server: " << optResult << std::endl;

    if (UDT::ERROR == UDT::bind(serv, (sockaddr*)&my_addr, sizeof(my_addr)))
    {
        std::cout << "bind: " << UDT::getlasterror().getErrorMessage();
        exit (0);
    }

    UDT::listen(serv, 10);

    int namelen;
    sockaddr_in their_addr;

    recver = UDT::accept(serv, (sockaddr*)&their_addr, &namelen);    
   
    char ip[16];
    std::cout << "new connection: " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << std::endl;

    //if (UDT::ERROR == UDT::connect(serv, (sockaddr*)&their_addr, sizeof(their_addr)))
    //{
        //std::cout << "connect send server to cleint: " << UDT::getlasterror().getErrorMessage();
        //exit(0);
    //}
    ConfigureServerOptions();
}

void ShowCCSignature()
{
    std::cout << "ShowCCSignature" << std::endl;
    int temp;
    UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
    if(cchandle != NULL)
        cchandle->Signature();
    else
         std::cout << "Não acionou o metodo" << std::endl;
}

std::streampos Get_file_size()
{
    //std::ifstream& ifs("//home//ns//ns-share//video_test.mp4");
    std::ifstream ifs(filePath);
    ifs.seekg(0, std::ios::end);
    std::streampos size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    ifs.close();
    return size;
}

void deal_with_options(string par_string, int par_argc, char* par_argv[])
{

    class_mrs_debug::print("par_string: ",par_string);

    

    if(par_string.find("say_hello") != string::npos)    
    {
        cout << "atomic args. It's Ok." << endl;
        return;
    }
    


    if(par_string.find("server") != string::npos)    
    {
        if(par_argc != 3)
        {
            std::cout << "We need ${app_var} ${door}"  << endl;
            exit(0);
        }        

        std::cout << "Server Port: " << par_argv[2]<<"\n";
        server_port = std::stoi(string(par_argv[2]));     
        return;
    }
    
    if(par_string == "udt_client" || par_string == "tcp_client")
    {

        //${door} Treino 100 ${num_clients} ${start_date}

        if(par_argc != 8)
        {
            std::cout << "We need ${app_var} ${door} ${cong_cont_var} ${data_type_var} ${rate_var} ${flows} ${date_var}" << endl;
            exit(0);
        }        

        std::cout << "Server Port: " << par_argv[2]<<"\n";
        server_port = std::stoi(string(par_argv[2]));     

        
        std::cout << "Congestion Control: " << par_argv[3]<<"\n";
        congestion_control = string(par_argv[3]);

        std::cout << "Participando de uma simulação do tipo : " << par_argv[4]<<"\n";
        tipo_dado = string(par_argv[4]);
        
        std::cout << "Data rate : " << par_argv[5]<<"\n";
        data_rate = string(par_argv[5]);

        std::cout << "Participando de uma simulação de : " << par_argv[6]<<" Fluxos\n";
        num_flows = string(par_argv[6]);

        std::cout << "Inicio da Simulacao " << par_argv[7]<<"\n";
        simu_start_time = string(par_argv[7]);
        std::replace(simu_start_time.begin(), simu_start_time.end(),':','_');
        std::replace(simu_start_time.begin(), simu_start_time.end(),' ','_');
        return;

    }

    if(par_string.find("extractor")!=string::npos)    
    {
        //${door} ${experiment_path}
        if(par_argc != 4)
        {
            std::cout << "We need ${app_var} ${door} ${path_var}" << endl;
            exit(0);
        }        
        std::cout << "Server Port: " << par_argv[2]<<"\n";
        server_port = std::stoi(string(par_argv[2])); 
        
        std::cout << "Experiment path: " << par_argv[3]<<"\n";
        experiment_path = string(par_argv[3]);

        return;


    }

    if(par_string == "router_ewma")
    {
        if(par_argc != 3)
        {
            std::cout << "We need ${app_var} ${path_val}" << endl;
            exit(0);
        }        


        std::cout << "Experiment path: " << par_argv[2]<<"\n";
        experiment_path = string(par_argv[2]);
        return;

    }

    cout << "!nvalid args!!" << endl;
    exit(0);



}


int main(int argc, char**argv){

 // ./communicator $app_type $port $cong_control $simul_type $data_rate $num_flows $simul_start_time

    bool main_force_print =false;

    printf("Hello, from communicator!\n");
    char c;
    //Socket* ptSocket=0;
    //bool server = true;
    //bool debug = true;
    std::cout << "Terminal Type: " << argv[1]<<"\n";
    terminal_type = string(argv[1]);

    
    deal_with_options(terminal_type,argc,argv);


    if(std::string(argv[1]) == "say_hello")
    {

        cout << "Hello!" << endl;

        uint64_t unidade = 10;
        uint64_t cauda = 1000000;
        uint64_t time = unidade*cauda; 

        cout << "before" << endl;
        usleep(time);
        cout << "afther" <<endl;

        return 1;
    }



    if(std::string(argv[1]) == "udt_client")
    {



        Set_client_socket();
        
        
        char hello[3000];// = "hello world!\n";
        int packets_sent = 0;
        int packets_to_send =  std::stoi(std::string(argv[2]));
        
        if(send_type == PCC)
        {
            cout << "PCC" << endl;
            //Pause();

            unsigned size =  transmission_size;
            char* data = new char[transmission_size];            
            //bzero(data, transmission_size);
            bone(data, transmission_size);
            bool first_send_aleready = false;
            int ssize = 0;
            int ss;
            std::time_t time_of_last_send;  
            std::time_t time_send; 
            //char ack[100];
            //int delay_quebra=0; //usada para pausar por um espaco de tempo em caso 
                                  //de quebra no send. Perde sentido quando o socket e SOCK_DGRAM,
                                  //pois nesse caso a condição nunca acontece.
            int random_number;//usado para dar pausas aleatórias entre as tranmissões
            int unlock_send = 0;//usada para liberar a transmissão, quando não
                                //é liberado no on ack ver linhas iniciais de on ack.
                                //ficou obsoleto 
            //unsigned numPack = 0;

            auto simulation_start = high_resolution_clock::now();
            auto simulation_time = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(simulation_time - simulation_start);
            while (true && duration.count() <= SIMUL_TIME_UDT)             
            {
                
                Pausar_por_tempo_aleatorio_micro_segundos(100000);//0.1s

                //esse if abaixo  visa regular o tempo entre envios para se 
                //atingir uma determinada velocidade
                /*if(first_send_aleready) //Já mandou? ja tem time_last_send
                {
                    //cout << "testing vel"<<"\n";
                    time_send = std::time(0);
                    //cout << "delta t: " <<  time_send - time_of_last_send <<"\n";
                    if((time_send - time_of_last_send) < 2)//regula o delta t
                        continue;
                }*/
                
                if(send_lock)
                {
                    cout << "opt_pure_udt!!!!&&&&&&&& "<< opt_pure_udt <<"\n";
                    cout << "send lock!!!!###### "<< (int) send_lock <<"\n";
                    cout << "unlock_send!!!!@@@ "<< unlock_send << (int) send_lock <<"\n";
                    unlock_send++;
                    Pausar_por_tempo_aleatorio_micro_segundos(2000);
                    //sleep(3);
                    if(unlock_send > 20)
                    {
                        unlock_send = 0;
                        send_lock = false;
                    }
                    
                    continue;
                }
                unlock_send = 0;
                cout << "+ envio (ssize,size,last_ack)" 
                <<"("<<ssize
                <<", "<<size
                <<", "<<last_ack
                <<")"<< "\n";
                ssize = 0;
                //while (ssize < size)//quando nao era datagrama. Ver projeto PCC
                //{
                    cout << "enviando..... "<< size <<"\n";
                    //sleep(2*delay_quebra); //quando nao era datagrama, fazia delay quando
                                             //quebrava no meio da transmissao, ou seja ssize < size 
                    //if (UDT::ERROR ==(ss = UDT::send(client, data + ssize, size - ssize, 0)))//quando nao era SOCK_DGRAM. PCC like 
                    if( UDT::ERROR == (ss = UDT::sendmsg(client, data, size, -1, true)))
                    {
                        cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
                        break;
                    }
                    cout << "ss: " << ss << "\n";
                    //cin >> c;
                    ssize += ss;
                    first_send_aleready = true;//agora é possível calcular delta t entre as transissões
                    //sleep(2);
                    time_of_last_send = std::time(0); //tempo do ultimo envio, para calcular delta t
                    //delay_quebra++;
                //}

                //delay_quebra = 0;
                if(opt_pure_udt < 0)//Se for puro udt (opt_pure_udt=0), não bloqueia.
                    send_lock = true;
                Pausar_por_tempo_aleatorio_micro_segundos(200);
                //numPack++;
                //cout << "numPack: " << numPack << endl;
                if (ssize < size) //é sianl que deu erro
                {
                    break;
                }

                simulation_time = high_resolution_clock::now();
                duration = duration_cast<microseconds>(simulation_time - simulation_start);
                cout << "duration: " << duration.count() << endl;
                //cin >> c;
            }
        
          cout << "Client is done! Tanks!!" <<endl;
          UDT::close(client);
          delete [] data;
          exit(0);

        }

        else if(send_type == MESSAGE)
        {

            while(packets_sent < packets_to_send)
            {
                for(int i=0; i < MESSAGE_SIZE-1; i++)
                    hello[i] = 'a';
                hello[MESSAGE_SIZE -1] ='\0';


                if (UDT::ERROR == UDT::sendmsg(client, hello,MESSAGE_SIZE, -1,false))
                //if (UDT::ERROR == UDT::send(client, hello, strlen(hello) + 1, 0))
                {
                    std::cout << "send: " << UDT::getlasterror().getErrorMessage();
                    return 0;
                }
                std::cout << "sending: " << hello << std::endl;
                packets_sent++;
                int rsize = UDT::recvmsg(client, hello, MESSAGE_SIZE);

                //sleep(3);
                
            }
        }

        else if (send_type == FILE)
        {
            
            int64_t  init;
            init = 0;
            std::fstream ifs(filePath);
            ifs.seekg(0, ios::end);
            std::streampos size = ifs.tellg();
            ifs.seekg(0, ios::beg);

            if (UDT::ERROR == UDT::sendfile(client, ifs, init, size))
            {
                std::cout << "sendfile: " << UDT::getlasterror().getErrorMessage();
                ifs.close();
                return 0;
            }
            ifs.close(); 
        }

        else // Just Hello
        {
            
            while(packets_sent < packets_to_send)
            {
                
                
                if (UDT::ERROR == UDT::send(client, hello, strlen(hello) + 1, 0))
                {
                    std::cout << "send: " << UDT::getlasterror().getErrorMessage();
                    return 0;
                }
                std::cout << "sending: " << hello << std::endl;
                packets_sent++;
                sleep(3);
            }

        }

        UDT::close(client);

        return 1;


    }


    if(std::string(argv[1]) == "udt_server" )
    {


        Set_server_socket();

        char data[100];

        if(send_type == PCC)
        {
            char* data;
            int size = MAX_PCC_SND_SIZE;
            data = new char[size];
            int rsize = 0;
            int rs;
            int rcv_size;
            int var_size = sizeof(int);


            while (true) 
            {
                rsize = 0; //Nada enviado nessa rodada de Tx
                while (rsize < size) 
                {
                // UDT::getsockopt(recver, 0, UDT_RCVDATA, &rcv_size, &var_size);
                    //if (UDT::ERROR == (rs = UDT::recv(recver, data + rsize, size - rsize, 0))) 
                    if(UDT::ERROR == (rs= UDT::recvmsg(recver, data, size)))
                    {
                        cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
                        break;
                    }

                    rsize += rs;
                }
            

                if (rsize < size) //sinal de erro
                {
                    break;
                }
            }
        

            delete [] data;
            UDT::close(recver);
        }

    
        else if(send_type == MESSAGE)
        {

            char messa_data[MESSAGE_SIZE];
            while(true)
            {
                
                if (UDT::ERROR == UDT::recvmsg(recver, data, MESSAGE_SIZE))
                //if (UDT::ERROR == UDT::recv(recver, data, 100, 0))
                {
                    std::cout << "recv:" << UDT::getlasterror().getErrorMessage() << std::endl;
                    return 0;
                }

                std::cout << data << std::endl;

                
            }

        }

        else if (send_type == FILE)
        {

            int64_t  init;
            init = 0;
            std::streampos size = Get_file_size();
            fstream ofs;
            ofs.open("largefile.mp4",ios::out | ios::trunc);
            if (UDT::ERROR == UDT::recvfile(recver, ofs, init, size))
            {
                std::cout << "recvfile: " << UDT::getlasterror().getErrorMessage();
                return 0;
            } 
            ofs.close();
        }

        else //just hello
        {
           while(true)
            {
                
                
                if (UDT::ERROR == UDT::recv(recver, data, 100, 0))
                {
                    std::cout << "recv:" << UDT::getlasterror().getErrorMessage() << std::endl;
                    return 0;
                }

                std::cout << data << std::endl;

                
            }

        }

        UDT::close(recver);
        UDT::close(serv);

        return 1;
    }

    if(std::string(argv[1]) == "tcp_server")
    {
        std::cout << "Starting TCP Socket" << std::endl;
        TCP_Server communicator;
        communicator.SetPort((u_int32_t)server_port); 
        communicator.SetOptions();
        communicator.Bind();
        communicator.Listen();
        communicator.Accept();
        communicator.Read();
        //communicator.Close();
        return 1;

    }
    if(std::string(argv[1]) == "tcp_client")
    {

        bool tcp_client_force_print = false;
        class_mrs_debug::print<char>("Instanciando cliente tcp", '\n', tcp_client_force_print);

        /****************** COMAND FORMAT*************************

        ./communicator tcp_client $cong_control $port $simul_type $data_rate $num_flows $simul_start_time
        
        **********************************************************/
        uint64_t mss = 60000;
        uint64_t clients_to_fill_band = 50; //stoull(num_flows);
        uint64_t simul_time = 90000000; //1.5 min em microsec

       TCP_Client communicator((u_int32_t)server_port,
                                clients_to_fill_band,
                                data_rate,
                                congestion_control,
                                mss,
                                num_flows,
                                simul_time,
                                tipo_dado,
                                simu_start_time);
        
        class_mrs_debug::print<char>("cliente tcp instanciado", '\n', tcp_client_force_print);

        class_report obj_report(communicator.get_experiment_dir(), "tcp_clients_report.txt");
        obj_report.meth_add_report_field_values("tipo_dado", tipo_dado);
        obj_report.meth_add_report_field_values("congestion_control", congestion_control);
        obj_report.meth_add_report_field_values("data_rate",to_string(communicator.transmission_rate_mbps));
        obj_report.meth_add_report_field_values("num_flows", num_flows);
        obj_report.meth_add_report_field_values("mss",to_string(communicator.mss));
        obj_report.meth_add_report_field_values("clients_to_fill_band",to_string(communicator.clients_to_fill_band));
        obj_report.meth_add_report_field_values("period_to_transmit",to_string(communicator.period_to_transmit_micro_seconds));
        obj_report.meth_report();

        class_mrs_debug::print<uint64_t>("period_to_transmit: ", communicator.get_period_to_transmit());
        
        communicator.SetPort((u_int32_t)server_port);
        communicator.SetServerAddr();
        communicator.ConnectToServer();
        communicator.Send();
        //communicator.Close();


        
        return 1;
    }

    if(std::string(argv[1]) == "feature_saver")
    {
        class_feature_saver_udt obj_saver_udt;
        obj_saver_udt.set_port(server_port);
        obj_saver_udt.meth_adjust_file_path();



        high_resolution_clock::time_point t1_ack;
        high_resolution_clock::time_point t2_ack;

        high_resolution_clock::time_point t1_snd;
        high_resolution_clock::time_point t2_snd;
        duration<double> interval_ack;
        duration<double> interval_snd;
        duration<double> rtt_duration;

        
        //"enviando os pacotes"
        cout << "enviando....." << endl;
        for(int i = 1; i < 10; i++)
        {
            obj_saver_udt.meth_deal_packet_send((uint32_t)i, high_resolution_clock::now());
            Pausar_por_tempo_aleatorio_micro_segundos(1500000);
        }


 
        //"chegando os acks"
        for (int i=2; i<=10; i++)
        {       
            cout <<"Round: "<< i << endl;
            t1_ack = t2_ack;            
            t2_ack = high_resolution_clock::now();

            t1_snd = t2_snd;
            t2_snd = map_seq_timestamp.at((uint32_t)i-1);
            obj_saver_udt.meth_deal_ack_arrival((uint32_t)i, t2_ack);
            Pausar_por_tempo_aleatorio_micro_segundos(1500000);
            rtt_duration = duration_cast<microseconds>(t2_ack - t2_snd);
            cout << "rtt: " << rtt_duration.count() << endl; 
            if(i > 2)
            {
                
 
                //t2_snd = map_seq_timestamp.at((uint32_t)i-1);
                interval_ack = duration_cast<microseconds>(t2_ack - t1_ack);
                interval_snd = duration_cast<microseconds>(t2_snd - t1_snd);
                cout << "Dt_ack: " << interval_ack.count() << "; ack_ewma: " << obj_saver_udt.get_ack_ewma() << std::endl;
                cout << "Dt_snd: " << interval_snd.count() << "; snd_ewma: " << obj_saver_udt.get_send_ewma() << std::endl;
            }
        }

        // Declaration
        //unordered_map<uint32_t, high_resolution_clock::time_point> mymap;
        

        //simulando a chegada de acks

        return 1;
    }

   
   
   
   if(std::string(argv[1]) == "drain_dump")
    {
        
        
        //class_feature_extractor obj_extractor;
        /*obj_extractor.set_port((uint32_t)server_port);
        obj_extractor.set_out_dir(experiment_path);
        obj_extractor.set_dump_file("udt_dump.txt");
        obj_extractor.set_queue_size_along_time_file("queue_along_time.txt");
        obj_extractor.meth_adjust_seq_metrics_file_path();
        */
       class_feature_extractor::meth_drain_dump_file(string("/home/ns/Desktop/output") + "/" + experiment_path + "/" + "router_data" + "/" + "udt_dump.txt");       
       return 1;
    
    }




    if(std::string(argv[1]) == "router_ewma")
    {
        
        class_mrs_debug::print<char>("entrou router_ewma",'\n');
        
        class_feature_extractor::meth_generate_queue_ewma_along_time_file(
            string("/home/ns/Desktop/output") + "/" + experiment_path + "/" + "router_data" + "/" + "queue_along_time.txt",
            string("/home/ns/Desktop/output") + "/" + experiment_path + "/" + "router_data" + "/" + "queue_along_time_ewma.txt");
      
      
        return 1;
    
    }

    if(std::string(argv[1]) == "udt_feature_extractor")
    {
        
        cout << "Caso feature_extractor" << endl;
        
        class_feature_extractor_udt obj_extractor_udt;
        obj_extractor_udt.set_port((uint32_t)server_port);
        
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        //o out_dir é o diretório onde foram salvos os dados dos
        //fluxos, que é o mesmo que deve ser usado para guardar
        //os dados do roteador.
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        obj_extractor_udt.set_out_dir(experiment_path);
        obj_extractor_udt.set_dump_file("udt_dump.txt");
        obj_extractor_udt.set_queue_size_along_time_file("queue_along_time.txt");//vai procurar o ewma
        obj_extractor_udt.meth_adjust_seq_metrics_file_path();
        obj_extractor_udt.meth_extract_router_features();//Gera o cvs.
   
        return 1;
    }


    if(std::string(argv[1]) == "tcp_feature_extractor")
    {
        
        cout << "Caso tcp_feature_extractor" << endl;
        
        class_feature_extractor_tcp obj_extractor_tcp;
        obj_extractor_tcp.set_port((uint32_t)server_port);
        
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        //o out_dir é o diretório onde foram salvos os dados dos
        //fluxos, que é o mesmo que deve ser usado para guardar
        //os dados do roteador.
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        obj_extractor_tcp.set_out_dir(experiment_path);
        obj_extractor_tcp.set_dump_file("tcp_dump.txt");
        obj_extractor_tcp.set_queue_size_along_time_file("queue_along_time.txt");//vai procurar o ewma
        obj_extractor_tcp.meth_adjust_seq_metrics_file_path();
        obj_extractor_tcp.meth_extract_router_features();//Gera o cvs.
   
        return 1;
    }



    if(std::string(argv[1]) == "udt_feature_extractor_tcp_test")
    {
        
        cout << "Caso feature_extractor_tcp test" << endl;
        
        class_feature_extractor_tcp obj_extractor_tcp;
        
        string dump_line = "1741115941.544244 IP 10.0.0.3.59492 > 10.0.1.3.9090: Flags [P.], seq 1:2897, ack 1, win 502, options [nop,nop,TS val 1796301299 ecr 2335579299], length 2896";
        
        
        
        cout << obj_extractor_tcp.meth_search_time_stamp(dump_line) << endl;

        string seq_number = obj_extractor_tcp.meth_search_seq_number(dump_line);

        cout << seq_number << endl;

        seq_number =  obj_extractor_tcp.meth_search_occurence_string_between_delimiter(seq_number,':',2);
        
        cout << seq_number << endl;

        return 1;
    }

    if(std::string(argv[1]) == "tcp_client_feature_extractor_test")
    {
        cout << "Caso tcp_feature_saver_test" << endl;

        char c;

        string vet_dump_lines[]{"1741180556.605556 IP 10.0.1.3.9090 > ns-virtual-machine.37170: Flags [.], ack 5793, win 592, options [nop,nop,TS val 2358321713 ecr 4149794421], length 0",
                                "1741180556.605558 IP 10.0.1.3.9090 > ns-virtual-machine.37170: Flags [.], ack 8689, win 637, options [nop,nop,TS val 2358321713 ecr 4149794421], length 0",
                                "1741180556.605683 IP 10.0.1.3.9090 > ns-virtual-machine.37170: Flags [.], ack 11585, win 682, options [nop,nop,TS val 2358321714 ecr 4149794421], length 0",
                                "1741180556.606030 IP 10.0.1.3.9090 > ns-virtual-machine.37170: Flags [.], ack 14481, win 681, options [nop,nop,TS val 2358321714 ecr 4149794421], length 0"
                               };

        int num_test = 4;

         class_feature_extractor_tcp_client obj_extractor;
         obj_extractor.set_port(server_port);
         uint64_t ack_arrival;
         uint64_t echo_reply;
         uint64_t seq_number;

         for (int i = 0; i < num_test; i++)
         { 
         obj_extractor.meth_extract_client_feature(vet_dump_lines[i],seq_number,ack_arrival,echo_reply);
         cout << "seq: " << seq_number << endl;
         cout << "ack_arrival: " << ack_arrival << endl;
         cout << "echo_reply: " << echo_reply << endl;
         cin >> c;
         }
         

        return 1;
    }


    if(std::string(argv[1]) == "tcp_feature_saver") //Analisar, parece obsoleto, 
                                                    //pois no tcp, o saver é junto com o
                                                    //extractro
    {
       /*
        cout << "Caso tcp_feature_saver" << endl;

        class_feature_extractor_tcp_client obj_extractor;
        string dump_line;
        obj_extractor.set_port(server_port);
        uint64_t ack_arrival;
        uint64_t echo_reply;
        uint64_t seq_number;
        uint64_t work_line;


        class_feature_saver_tcp obj_saver;
        obj_saver.set_port(server_port);
        obj_saver.set_bottleneck_datarate(data_rate);
        obj_saver.set_default_congestion("Vegas");
        obj_saver.set_num_flows(num_flows);
        obj_saver.set_tipo_dado(tipo_dado);
        obj_saver.set_starting_time(simu_start_time);
        obj_saver.meth_adjust_file_path();

         
         //Anteriormente, a cada chegada de ack o meth_deal_ack_arrival era acionado,
         //agora, mantendo-se a mesma lógica, o meth_deal_ack_arrival é acionado quando
         //se encontra um ack no dump.
         
        ifstream stream_dump_file (obj_saver.get_out_dir()+ "/" + "clients_data" + "/" + "clients_dump.txt");
        if (stream_dump_file.is_open())
        {
            while (getline (stream_dump_file,dump_line) )
            {
    
                
                class_mrs_debug::print<uint64_t>("workline: ", ++work_line);
                class_mrs_debug::print<string>("dump_line: ", dump_line);
                obj_extractor.meth_extract_client_feature(dump_line,seq_number,ack_arrival,echo_reply);
                obj_saver.meth_deal_ack_arrival(seq_number,ack_arrival,echo_reply);
    
            }

            stream_dump_file.close();

        }

        else
        {
            cout << "Can't open client dump file" << endl;
            exit(0);
        } 
        */
        return 1;
    }


    if(std::string(argv[1]) == "tcp_feature_saver_and_extractor")
    {
       
        /****************** COMAND FORMAT************************

            ./communicator tcp_feature_saver_and_extractor $port $path
        
        **********************************************************/
        

        /*
            agora, enquanto estiver salvando o sistema também
            estará extraindo, para extrair apenas os seqs correspondentes
            aos ack. O saver provoca o extractor
        */


        auto simulation_start = high_resolution_clock::now();

        
        cout << "Caso tcp_feature_saver_and_extractor" << endl;
        //A classe class_feature_extractor_tcp_client é praticamente
        //  um extrator de acks
        
        class_feature_extractor_tcp_client obj_extractor_tcp_client;
        string dump_line;
        obj_extractor_tcp_client.set_port(server_port);
        



        uint64_t ack_arrival;
        uint64_t echo_reply;
        uint64_t seq_number;
        uint64_t work_line;



        class_feature_saver_tcp obj_saver;

        string tcp_saver_extractor_experiment_path = experiment_path;

        class_mrs_debug::print<string>("tcp_saver_extractor_experiment_path: ", tcp_saver_extractor_experiment_path);

        string tcp_saver_extractor_tipo_dado = class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',1);
        class_mrs_debug::print<string>("tcp_saver_extractor_tipo_dado: ", tcp_saver_extractor_tipo_dado);
        
        string tcp_saver_extractor_congestion_control = class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',2);
        class_mrs_debug::print<string>("tcp_saver_extractor_congestion_control:" ,tcp_saver_extractor_congestion_control);

        string tcp_saver_extractor_num_flows = class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',3);
        tcp_saver_extractor_num_flows = tcp_saver_extractor_num_flows.substr(0,tcp_saver_extractor_num_flows.find("Fluxos"));
        class_mrs_debug::print<string>("tcp_saver_extractor_num_flows: ",tcp_saver_extractor_num_flows);
        


        string tcp_saver_extractor_data_rate = class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',4);
        tcp_saver_extractor_data_rate = tcp_saver_extractor_data_rate.substr(0,tcp_saver_extractor_num_flows.find("Mbps")); 
        
        class_mrs_debug::print<string>("tcp_saver_extractor_data_rate: ",tcp_saver_extractor_data_rate);

        string tcp_saver_extractor_simul_start_time =   class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',5) 
                                                      +"_" + class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',6)
                                                      +"_" + class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',7)
                                                      +"_" + class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',8)
                                                      +"_" + class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',9)
                                                      +"_" + class_feature_extractor::meth_search_occurence_string_between_delimiter(tcp_saver_extractor_experiment_path,'_',10);

        
        class_mrs_debug::print<string>("tcp_saver_extractor_simul_start_time: ",tcp_saver_extractor_simul_start_time);
        
        obj_saver.set_port((uint32_t)server_port);
        obj_saver.set_bottleneck_datarate(tcp_saver_extractor_data_rate);
        obj_saver.set_default_congestion(tcp_saver_extractor_congestion_control);
        obj_saver.set_num_flows(tcp_saver_extractor_num_flows);
        obj_saver.set_tipo_dado(tcp_saver_extractor_tipo_dado);
        obj_saver.set_starting_time(tcp_saver_extractor_simul_start_time); //Aqui pega do ditetorio que foi criado na criação dos fluxos
        obj_saver.meth_adjust_file_path(tcp_saver_extractor_experiment_path);
        
        //é o saver que define o out_dir da forma "Treino_udt_60Fluxos_100Mbps_Fri_Mar_06_03_16_35"
        
        
        
        
        ifstream stream_dump_file (obj_saver.get_out_dir()+ "/" + "clients_data" + "/" + "clients_dump_ACK.txt");
        
        cout << "client ifstream: " << obj_saver.get_out_dir()+ "/" + "clients_data" + "/" + "clients_dump_ACK.txt" << endl;
        
        class_feature_extractor_tcp obj_extractor_tcp_router;
        obj_extractor_tcp_router.set_port((uint32_t)server_port);
        
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        //o out_dir é o diretório onde foram salvos os dados dos
        //fluxos, que é o mesmo que deve ser usado para guardar
        //os dados do roteador.
        //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
                                             
        obj_extractor_tcp_router.set_out_dir(tcp_saver_extractor_experiment_path);
        obj_extractor_tcp_router.set_dump_file("tcp_dump.txt");
        obj_extractor_tcp_router.set_queue_size_along_time_file("queue_along_time.txt");//vai procurar o ewma
        obj_extractor_tcp_router.meth_adjust_seq_metrics_file_path();
        if(!obj_extractor_tcp_router.meth_open_dump_file())
        {
            cout << "Can't open packet dump file" << endl;
            exit(0);
        }

        uint64_t syn_time_stamp;
        uint64_t TS_val;
        obj_extractor_tcp_client.set_SYN_file(obj_saver.get_out_dir() + "/" + "clients_data" + "/" + "clients_dump_SYN.txt");
        
        if(obj_extractor_tcp_client.meth_give_time_stamp_to_synchronize_clock(syn_time_stamp, TS_val))
        {
            cout << "Setting Virtual Clock origin" << endl;
            obj_saver.meth_set_virtual_clock_origin(syn_time_stamp,TS_val);


        }
        else
        {
            cout << "Can't set virtual clock origin";
            return 1;
        }

        bool intersting_line = false;
        if (stream_dump_file.is_open())
        {
            
            cout << "ACK File dump opened." << endl;
            
            
            while (getline (stream_dump_file,dump_line) )
            {
                
                class_mrs_debug::print<uint64_t>("workline in client dump file: ", ++work_line,main_force_print);
                class_mrs_debug::print<string>("dump_line in client dump file: ", dump_line, main_force_print);
                
                if(!obj_extractor_tcp_router.meth_more_packet_line())
                {
                    cout << "No more packet lines!" << endl;
                    break;
                }
                
                //Verifica se é linha do fluxo da porta
                if(dump_line.find("10.0.1.3." + to_string(server_port)) == std::string::npos)
                {
                    class_mrs_debug::print<string>("line is not of my flow: ", dump_line,main_force_print);
                    //cout << "not intersting line." << endl;
                    continue;
                }
                intersting_line = true;
                //cin.ignore();
                //extrai os tempos dos pacotes ack
                //No futuro pensar na possibilidade de se fazer um extractor só, com o tcp_client herdando do tcp_router
                obj_extractor_tcp_client.meth_update_port_state(dump_line);//São asportas esperadas?
                if(obj_extractor_tcp_client.get_port_changed())//Se a porta mudou, não são!
                {
                    //Então busco o pacote com a porta certa.
                    if(!obj_extractor_tcp_router.deal_with_port_change(obj_extractor_tcp_client.get_current_port()))
                    {   
                        //Tem horas que muda, mas não há mais pacotes na porta para
                        //qual mudou. Impressionante!
                        cout << "Can not change port." <<endl;
                        break;

                    }
                    //A principio, nao ha mais mudança de porta, pois as portas 
                    //dos clientess foram fixadas. Mesmp assim, o iperf faz bagunça
                    //as vezes
                    continue;


                }
                
                if(obj_extractor_tcp_client.meth_extract_client_feature(dump_line,seq_number,ack_arrival,echo_reply) && class_feature_saver_tcp::meth_clock_origin_configured())
                {
                    if(obj_extractor_tcp_router.meth_extract_router_features(seq_number))//Gera o cvs. seq-router_ewma
                    {   
                        //só alimenta os arquvivo dos vetores de treinamento, se o ack foi efetivamente
                        //associado a um ack ewma.
                        class_mrs_debug::print<uint64_t>("seq to save: ",seq_number, main_force_print);
                        class_mrs_debug::print<uint64_t>("ack_arival to save: ", ack_arrival,main_force_print);
                        class_mrs_debug::print<uint64_t>("ech reply to save: ",echo_reply,main_force_print);
                        obj_saver.meth_deal_ack_arrival(seq_number,ack_arrival,echo_reply);                    
                        class_mrs_debug::print<char>("extraiu features efetivamente ", '\n',main_force_print);
                    }
                }
                else
                {
                    if(dump_line.find("[S],")!=std::string::npos &&
                       dump_line.find(string(" > ") + "10.0.1.3." + to_string(server_port))!= std::string::npos)
                       {
                            class_mrs_debug::print<char>("Setting virtual clock origin ", '\n',main_force_print);
                            string syn_time_stamp = class_feature_extractor::meth_search_occurence_string_between_delimiter(dump_line,' ',1);
                            class_feature_extractor::meth_erase_dot_from_time_stamp(syn_time_stamp);
                            string syn_TS_val = class_feature_extractor::meth_search_occurence_string_between_delimiter(dump_line,' ',16);
                            class_mrs_debug::print<string>("syn_time_stamp: ", syn_time_stamp,main_force_print);
                            class_mrs_debug::print<string>("sys_TS_val: ", syn_TS_val,main_force_print);
                            obj_saver.meth_set_virtual_clock_origin(stoull(syn_time_stamp),stoull(syn_TS_val));
                            
                       }

                    else if(!class_feature_saver_tcp::meth_clock_origin_configured() && dump_line.find("[S.],")!=std::string::npos &&
                    dump_line.find(string("10.0.1.3.") + to_string(server_port)+" > 10.0.0.3")!= std::string::npos)
                    {
                        class_mrs_debug::print<char>("Setting virtual clock origin by [S.]", '\n',main_force_print);
                        string syn_time_stamp = class_feature_extractor::meth_search_occurence_string_between_delimiter(dump_line,' ',1);
                        class_feature_extractor::meth_erase_dot_from_time_stamp(syn_time_stamp);
                        //20 para pegar o ecro reply....ecr
                        string syn_TS_val = class_feature_extractor::meth_search_occurence_string_between_delimiter(dump_line,' ',20);
                        syn_TS_val = class_feature_extractor::meth_search_occurence_string_between_delimiter(syn_TS_val,',',1);
                        class_mrs_debug::print<string>("syn_time_stamp: ", syn_time_stamp,main_force_print);
                        class_mrs_debug::print<string>("sys_TS_val: ", syn_TS_val,main_force_print);

                        //O 42500 a seguir é exatamente o RTT, ou seja o SYN saiu RTT microssegundos atrás.
                        obj_saver.meth_set_virtual_clock_origin(stoull(syn_time_stamp) - 42500,stoull(syn_TS_val));

                    }
                 }
    
                class_mrs_debug::print<char>("just a pause: ", '\n',main_force_print);
            
            }

            auto simulation_time = high_resolution_clock::now();
            auto duration = duration_cast<seconds>(simulation_time - simulation_start);

            cout << "Afther " << duration.count() <<  "No more Acks to me. Good bye!" << endl;
            cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" <<endl;
            cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" <<endl;
            cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" <<endl;
            stream_dump_file.close();


        }

        else
        {
            cout << "Can't open client dump ACK file" << endl;
            exit(0);
        } 


        if(!intersting_line)
            cout << "No line to considere for this port." <<endl;
        return 1;
    }



    cout << "Invalid option!!!\n" <<endl;

}
