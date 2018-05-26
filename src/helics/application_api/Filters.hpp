/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/
#pragma once

#include "../core/Core.hpp"
#include "Federate.hpp"
#include "helics/helics-config.h"

namespace helics
{
class FilterOperations;

/** a set of common defined filters*/
enum class defined_filter_types
{
    custom = 0,
    delay = 1,
    randomDelay = 2,
    randomDrop = 3,
    reroute = 4,
    clone = 5,
    unrecognized = 6

};

/** get the filter type from a string*/
defined_filter_types filterTypeFromString (const std::string &filterType) noexcept;

/** class for managing a particular filter*/
class Filter
{
  protected:
    Core *corePtr = nullptr;  //!< the Core to use
    handle_id_t id;  //!< the id as generated by the Federate
    filter_id_t fid = invalid_id_value;  //!< id for interacting with a federate
    std::shared_ptr<FilterOperations> filtOp;  //!< a class running any specific operation of the Filter
  public:
    /** default constructor*/
    Filter () = default;
    /** construct through a federate*/
    explicit Filter (Federate *fed);
    /** construct through a core object*/
    explicit Filter (Core *cr);

    /** virtual destructor*/
    virtual ~Filter () = default;

    /** set a message operator to process the message*/
    void setOperator (std::shared_ptr<FilterOperator> mo);

    /** get the underlying filter id for use with a federate*/
    filter_id_t getID () const { return fid; }

    /** get the underlying core handle for use with a core*/
    handle_id_t getCoreHandle () const { return id; }

    /** get the target of the filter*/
    const std::string &getTarget () const;
    /** get the name for the filter*/
    const std::string &getName () const;
    /** get the specified input type of the filter*/
    const std::string &getInputType () const;
    /** get the specified output type of the filter*/
    const std::string &getOutputType () const;
    /** set a property on a filter
    @param property the name of the property of the filter to change
    @param val the numerical value of the property
    */
    virtual void set (const std::string &property, double val);
    /** set a string property on a filter
    @param property the name of the property of the filter to change
    @param val the numerical value of the property
    */
    virtual void setString (const std::string &property, const std::string &val);

  protected:
    /** set a filter operations object */
    void setFilterOperations (std::shared_ptr<FilterOperations> filterOps);
    friend void addOperations (Filter *filt, defined_filter_types type, Core *cptr);
};

#define EMPTY_STRING std::string ()
/** class wrapping a source filter*/
class SourceFilter : public Filter
{
  public:
    /**constructor to build an source filter object
    @param[in] fed  the Federate to use
    @param[in] target the endpoint the filter is targeting
    @param[in] name the name of the filter
    @param[in] input_type the type of data the filter is expecting
    @param[in] output_type the type of data the filter is generating
    */
    SourceFilter (Federate *fed,
                  const std::string &target,
                  const std::string &name = EMPTY_STRING,
                  const std::string &input_type = EMPTY_STRING,
                  const std::string &output_type = EMPTY_STRING);
    /**constructor to build an source filter object
    @param[in] fed  the Federate to use
    @param[in] target the endpoint the filter is targeting
    @param[in] name the name of the filter
    @param[in] input_type the type of data the filter is expecting
    @param[in] output_type the type of data the filter is generating
    */
    SourceFilter (Core *cr,
                  const std::string &target,
                  const std::string &name = EMPTY_STRING,
                  const std::string &input_type = EMPTY_STRING,
                  const std::string &output_type = EMPTY_STRING);
    virtual ~SourceFilter () = default;
};

/** class wrapping a destination filter*/
class DestinationFilter : public Filter
{
  public:
    /**constructor to build an destination filter object
    @param[in] fed  the MessageFederate to use
    @param[in] target the endpoint the filter is targeting
    @param[in] name the name of the filter
    @param[in] input_type the type of data the filter is expecting
    @param[in] output_type the type of data the filter is generating
    */
    DestinationFilter (Federate *fed,
                       const std::string &target,
                       const std::string &name = EMPTY_STRING,
                       const std::string &input_type = EMPTY_STRING,
                       const std::string &output_type = EMPTY_STRING);
    /**constructor to build an destination filter object
    @param[in] cr  the Core to register the filter with
    @param[in] target the endpoint the filter is targeting
    @param[in] name the name of the filter
    @param[in] input_type the type of data the filter is expecting
    @param[in] output_type the type of data the filter is generating
    */
    DestinationFilter (Core *cr,
                       const std::string &target,
                       const std::string &name = EMPTY_STRING,
                       const std::string &input_type = EMPTY_STRING,
                       const std::string &output_type = EMPTY_STRING);
    virtual ~DestinationFilter () = default;
};

/** class used to clone message for delivery to other endpoints*/
class CloningFilter : public Filter
{
  public:
    /** construct from a core object
     */
    explicit CloningFilter (Core *cr);
    /** construct from a Federate
     */
    explicit CloningFilter (Federate *fed);

    /** add a sourceEndpoint to the list of endpoint to clone*/
    void addSourceTarget (const std::string &sourceName);
    /** add a destination endpoint to the list of endpoints to clone*/
    void addDestinationTarget (const std::string &destinationName);
    /** add a delivery address this is the name of an endpoint to deliver the message to*/
    void addDeliveryEndpoint (const std::string &endpoint);

    /** remove a sourceEndpoint to the list of endpoint to clone*/
    void removeSourceTarget (const std::string &sourceName);
    /** remove a destination endpoint to the list of endpoints to clone*/
    void removeDestinationTarget (const std::string &destinationName);
    /** remove a delivery address this is the name of an endpoint to deliver the message to*/
    void removeDeliveryEndpoint (const std::string &endpoint);

    virtual void setString (const std::string &property, const std::string &val) override;

  private:
    std::vector<filter_id_t> sourceFilters;  //!< the set of source filters to control
    std::vector<filter_id_t> destFilters;  //!< the set of destination Filters contained in the filter
    std::vector<std::string> sourceEndpoints;  //!< the names of the source endpoints
    std::vector<std::string> destEndpoints;  //!< the names of the destination endpoints
};

/** create a destination filter
@param type the type of filter to create
@param fed the federate to create the filter through
@param target the target endpoint all message with the specified target as a destination will route through the
filter
@param name the name of the filter (optional)
@return a unique pointer to a destination Filter object,  note destroying the object does not deactivate the filter
*/
std::unique_ptr<DestinationFilter> make_destination_filter (defined_filter_types type,
                                                            Federate *fed,
                                                            const std::string &target,
                                                            const std::string &name = EMPTY_STRING);
/** create a source filter
@param type the type of filter to create
@param fed the federate to create the filter through
@param target the target endpoint all message coming from the specified source will route through the filter
@param name the name of the filter (optional)
@return a unique pointer to a source Filter object,  note destroying the object does not deactivate the filter
*/
std::unique_ptr<SourceFilter> make_source_filter (defined_filter_types type,
                                                  Federate *fed,
                                                  const std::string &target,
                                                  const std::string &name = EMPTY_STRING);

/** create a destination filter
@param type the type of filter to create
@param cr the core to create the federate through
@param target the target endpoint all message with the specified target as a destination will route through the
filter
@param name the name of the filter (optional)
@return a unique pointer to a destination Filter object,  note destroying the object does not deactivate the filter
*/
std::unique_ptr<DestinationFilter> make_destination_filter (defined_filter_types type,
                                                            Core *cr,
                                                            const std::string &target,
                                                            const std::string &name = EMPTY_STRING);

/** create a source filter
@param type the type of filter to create
@param cr the core to create the filter through
@param target the target endpoint all message coming from the specified source will route through the filter
@param name the name of the filter (optional)
@return a unique pointer to a source Filter object,  note destroying the object does not deactivate the filter
*/
std::unique_ptr<SourceFilter> make_source_filter (defined_filter_types type,
                                                  Core *cr,
                                                  const std::string &target,
                                                  const std::string &name = EMPTY_STRING);

}  // namespace helics
