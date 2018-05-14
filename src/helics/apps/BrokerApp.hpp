/*
Copyright � 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include <memory>

namespace helics
{
class Broker;

namespace apps
{
/** class implementing a Broker object.  This object is meant to a be a very simple broker executor with a similar interface to the other apps
*/
class BrokerApp
{
public:
    /** default constructor*/
    BrokerApp() = default;
    /** construct from command line arguments
    @param argc the number of arguments
    @param argv the strings in the input
    */
    BrokerApp(int argc, char *argv[]);
   

    /** move construction*/
    BrokerApp(BrokerApp &&other_echo) = default;
    /** move assignment*/
    BrokerApp &operator= (BrokerApp &&brokerApp) = default;
    /** the destructor will wait until the broker is finished before returning- unless forceTerminate() is used*/
    ~BrokerApp();

    /** run the Echo federate until the specified time
    @param stopTime_input the desired stop time
    */
    /** check if the Broker is running*/
    bool isActive() const;

    /** forceably disconnect the broker*/
    void forceTerminate();

    /** overload the -> operator so broker functions can be called if needed
    */
    auto *operator->() const { return broker.operator->(); }

private:
    std::shared_ptr<Broker> broker;  //!< the actual endpoint objects
};
}
}
