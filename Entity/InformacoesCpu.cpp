//
// Created by eduardo on 14/09/17.
//

#include "InformacoesCpu.h"
#include "../Util/ConfigFile/ConfigFile.h"
#include "../Util/resource/Result.h"
#include "../Util/resource/Resource.h"
#include <boost/algorithm/string.hpp>

using namespace std;


InformacoesCpu::InformacoesCpu() {
};

InformacoesCpu::~InformacoesCpu() {

}


void InformacoesCpu::lerInformacoesCpu(){

    try {
        chdir("/proc/");
        ConfigFile configFile("cpuinfo", ":");
        configFile.load();

        setNome(configFile.getString("model name"));

        string cacheSize =  configFile.getString("cache size");
        boost::erase_all(cacheSize, " KB");
        setCacheSize(std::stol(cacheSize,nullptr,0));

        setCpuCores(configFile.getInt("cpu cores"));
        setSiblings(configFile.getInt("siblings"));

    } catch (FileNotFoundException &ex) {
        std::cerr << ex.what() << std::endl;
    } catch (InvalidPropertyName &ex) {
        std::cerr << ex.what() << std::endl;
    }
}


void InformacoesCpu::monitorarInformacoesCpu(ServidorConfig *srvConfig){

    Result *result = nullptr;
    do{
        if(result != nullptr){
            delete result;
        }

        servidor.setId(srvConfig->getServidor().getId());

        string path = "/servidor/"+ to_string(srvConfig->getServidor().getId()) +"/informacoescpu/";

        Resource resource(path, srvConfig->getHostMonitoramento(), srvConfig->getPorta());

        lerInformacoesCpu();

        result = resource.post(toJson());
        if(result->getStatus() == 200){
            fromJson(result->getResult());
        }
        result->imprimir("InformacoesCpu");
    }
    while(result->getStatus() != 200);
    delete result;
}

std::string InformacoesCpu::toJson(){
    ptree pt;
    pt.put("servidor.id", servidor.getId());
    pt.put("nome", getNome());
    pt.put("cacheSize", getCacheSize());
    pt.put("cpuCores", getCpuCores());
    pt.put("siblings", getSiblings());

    std::ostringstream buf;
    write_json (buf, pt, false);
    string json = buf.str();
    return  json;
}

bool InformacoesCpu::fromJson(const std::string &json){
    ptree pt2;
    std::istringstream is (json);
    read_json (is, pt2);

    setId(pt2.get<long> ("id"));
    servidor.setId(pt2.get<long> ("servidor.id"));
    servidor.setNome(pt2.get<string> ("servidor.nome"));
    setNome(pt2.get<string> ("nome"));
    setCacheSize(pt2.get<long> ("cacheSize"));
    setCpuCores(pt2.get<long> ("cpuCores"));
    setSiblings(pt2.get<long> ("siblings"));

    return true;
}

void InformacoesCpu::print(){
    cout << "Id: " << getId() << endl;
    cout << "servidor.id: " << servidor.getId() << endl;
    cout << "servidor.nome: " << servidor.getNome() << endl;
    cout << "nome: " << getNome() << endl;
    cout << "size: " << getCacheSize() << endl;
    cout << "cores: " << getCpuCores() << endl;
    cout << "siblings : " << getSiblings() << endl;
}





