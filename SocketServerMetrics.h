//
// Created by goforbroke on 22.05.2021.
//

#ifndef SERVER_SAMPLES_SOCKETSERVERMETRICS_H
#define SERVER_SAMPLES_SOCKETSERVERMETRICS_H

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <regex>

std::string get_metric_name(const std::string &name) {
    std::string result = name;
    result = std::regex_replace(result, std::regex("-"), "_");
    return result;
}

class SocketServerMetrics {

public:

    SocketServerMetrics(const std::string &serviceName, const std::string &componentName)
            : registry(std::make_shared<prometheus::Registry>()),
              acceptedClientsCounter(
                      prometheus::BuildCounter().Labels({{"component", componentName}}).Name(
                              serviceName + "_" + "accepted_clients").Help(
                              "Number of accepted clients").Register(*this->registry)
              ),
              processedMessagesCounter(
                      prometheus::BuildCounter().Labels({{"component", componentName}}).Name(
                              serviceName + "_" + "processed_messages").Help(
                              "Number of processed messages").Register(*this->registry)
              ) {}

    void init(const std::string &address = "0.0.0.0:8080") {
        this->exposer = std::make_shared<prometheus::Exposer>(address);
        this->exposer->RegisterCollectable(registry);
    }

    prometheus::Counter &acceptedClients() {
        return this->acceptedClientsCounter.Add({});
    }

    prometheus::Counter &processedMessages() {
        return this->processedMessagesCounter.Add({});
    }

private:
    std::shared_ptr<prometheus::Exposer> exposer;
    std::shared_ptr<prometheus::Registry> registry;

    prometheus::Family<prometheus::Counter> &acceptedClientsCounter;
    prometheus::Family<prometheus::Counter> &processedMessagesCounter;
};


#endif //SERVER_SAMPLES_SOCKETSERVERMETRICS_H
