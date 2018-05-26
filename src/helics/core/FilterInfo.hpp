/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/
#pragma once

#include "Core.hpp"
#include "helics-time.hpp"
#include "helics/helics-config.h"

#include <memory>
#include <utility>

namespace helics
{
/** data class defining the information about a filter*/
class FilterInfo
{
  public:
    /** constructor from all fields*/
    FilterInfo (global_broker_id_t core_id_,
                handle_id_t handle_,
                const std::string &key_,
                const std::string &target_,
                const std::string &type_in_,
                const std::string &type_out_,
                bool destFilter_)
        : core_id (core_id_), handle (handle_), key (key_), filterTarget (target_), inputType (type_in_),
          outputType (type_out_), dest_filter (destFilter_)
    {
    }
    const global_broker_id_t core_id;  //!< id of the core that manages the filter
    const handle_id_t handle;  //!< id handle of the filter

    const std::string key;  //!< the identifier of the filter
    const std::string filterTarget;  //!< the target endpoint name of the filter
    const std::string inputType;  //!< the type of data for the filter
    const std::string outputType;  //!< the outputType of data of the filter
    const bool dest_filter = false;  //! indicator that the filter is a destination filter
    bool cloning = false;  //!< indicator that the filter is a cloning filter
    // there is a 6 byte gap here
    std::shared_ptr<FilterOperator> filterOp;  //!< the callback operation of the filter

    std::pair<global_federate_id_t, handle_id_t> target{
      global_federate_id_t(), handle_id_t()};  //!< the actual target information for the filter
};
}  // namespace helics
