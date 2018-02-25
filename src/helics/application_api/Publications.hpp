/*
Copyright (C) 2017-2018, Battelle Memorial Institute
All rights reserved.

This software was co-developed by Pacific Northwest National Laboratory, operated by the Battelle Memorial
Institute; the National Renewable Energy Laboratory, operated by the Alliance for Sustainable Energy, LLC; and the
Lawrence Livermore National Laboratory, operated by Lawrence Livermore National Security, LLC.
*/

#pragma once

#include "HelicsPrimaryTypes.hpp"
#include "ValueFederate.hpp"

namespace helics
{
class PublicationBase
{
  protected:
    ValueFederate *fed = nullptr;  //!< the federate construct to interact with
    publication_id_t id;  //!< the internal id of the publication
  private:
    std::string key_;  //!< the name of the publication
    std::string type_;  //!< the type of the publication
    std::string units_;  //!< the defined units of the publication
  public:
    PublicationBase () = default;
    /** base constructor for a publication
    @tparam avalueFed a pointer of some kind to a value federate (any dereferencable type with * and -> operator that
    results in a valueFederate object
    @param key the identifier for the publication
    @param type the type of the publication
    @param units, an optional string defining the units*/
    template<class FedPtr>
    PublicationBase (FedPtr valueFed,
                     const std::string &key,
                     const std::string &type,
                     const std::string &units = std::string())
        : fed (std::addressof(*valueFed)), key_ (key), type_ (type), units_ (units)
    {
        static_assert(std::is_base_of<ValueFederate, std::remove_reference_t<decltype(*valueFed)>>::value, "first argument must be a pointer to a ValueFederate");
        id = fed->registerPublication (key_, type_, units_);
    }

    /** base constructor for a publication
    @param locality either GLOBAL or LOCAL, LOCAL prepends the federate name to create a global identifier
    @tparam valueFed a pointer of some kind to a value federate (any dereferencable type with * and -> operator that
    results in a valueFederate object
    @param key the identifier for the publication
    @param type the type of the publication
    @param units, an optional string defining the units*/
    template<class FedPtr>
    PublicationBase (interface_visibility locality,
                     FedPtr valueFed,
                     const std::string &key,
                     const std::string &type,
                     const std::string &units = std::string())
        : fed (std::addressof(*valueFed)), key_ (key), type_ (type), units_ (units)
    {
        if (locality == GLOBAL)
        {
            id = fed->registerGlobalPublication (key, type, units);
        }
        else
        {
            id = fed->registerPublication (key, type, units);
        }
    }
    /** generate a publication object from an existing publication in a federate
    @Details useful for creating publication objects from publications generated by a configuration script
    */
    PublicationBase (ValueFederate *valueFed, int pubIndex);
    /** default destructor*/
    virtual ~PublicationBase () = default;

    /** get the publication id that can be used to make the function calls from a Value Federate
    */
    publication_id_t getID () const { return id; }

    /** get the key for the subscription*/
    std::string getKey () const { return fed->getPublicationKey (id); }
    /** get the key for the subscription*/
    const std::string &getName () const { return key_; }
    /** get the key for the subscription*/
    const std::string &getType () const { return type_; }
    /** get the units of the Publication*/
    const std::string &getUnits () const { return units_; }
};

/** class wrapping the calls for a publication in an object so identifiers and pointers do not 
need to be used for every call
*/
class Publication : public PublicationBase
{
  private:
    double delta = -1.0;  //!< the minimum change to publish
    helics_type_t pubType;  //!< the type of publication
    bool changeDetectionEnabled = false;  //!< the change detection is enabled

    mutable defV prevValue;  //!< the previous value of the publication
  public:
      /** default constructor*/
      Publication() = default;
    /**constructor to build a publication object
    @param[in] valueFed  the ValueFederate to use
    @param[in] key the identifier for the publication
    @param type_ the defined type of the publication
    @param[in] units the units associated with a Federate
    */
    template<class FedPtr>
    Publication(FedPtr valueFed, const std::string &key, helics_type_t type, const std::string &units = std::string())
        : PublicationBase (valueFed, key, typeNameStringRef (type), units), pubType (type)
    {
    }
    /**constructor to build a publication object
    @param locality  set to global for a global publication or local for a local one
    @param[in] valueFed  the ValueFederate to use
    @param[in] key the identifier for the publication
    @param type_ the defined type of the publication
    @param[in] units the units associated with a Federate
    */
    template<class FedPtr>
    Publication (interface_visibility locality,
                 FedPtr valueFed,
                 const std::string &key,
                 helics_type_t type,
                 const std::string &units = std::string())
        : PublicationBase (locality, valueFed, key, typeNameStringRef (type), units), pubType (type)
    {
    }
    /** generate a publication object from a preexisting publication
    @param valueFed a pointer to the appropriate value Federate
    @param pubIndex the index of the subscription
    */
    Publication (ValueFederate *valueFed, int pubIndex) : PublicationBase (valueFed, pubIndex),pubType(getTypeFromString(getType())) {}
    /** send a value for publication
    @param[in] val the value to publish*/
    void publish (double val) const;
    void publish (int64_t val) const;
    void publish (const char *val) const;
    void publish (const std::string &val) const;
    void publish (const std::vector<double> &val) const;
    void publish (const std::vector<std::complex<double>> &val) const;
    void publish (const double *vals, int size) const;
    void publish (std::complex<double> val) const;
    void publish (const defV &val) const;
    /** secondary publish function to allow unit conversion before publication
    @param[in] val the value to publish
    @param[in] units  the units association with the publication
    */
    template <class X>
    void publish (const X &val, const std::string & /*units*/) const
    {
        // TODO:: figure out units
        publish (val);
    }

    /** set the level by which a value must have changed to actually publish the value
    */
    void setMinimumChange (double deltaV)
    {
        if (delta < 0.0)
        {
            changeDetectionEnabled = true;
        }
        delta = deltaV;
        if (delta < 0.0)
        {
            changeDetectionEnabled = false;
        }
    }
    /** if set to false, the change detection mechanisms are not enabled
    if set to true the values will be published if there is sufficient change as specified in 
    the call to setMinimumChange
    */
    void enableChangeDetection (bool enabled = true) { changeDetectionEnabled = enabled; }

  private:
};

/** create a pointer to a publication
@tparam X is the type of the publication
@tparam FedPtr a pointer a value Federate
@param valueFed pointer to a valid federate
@param key the identifier for the publication
@param units optional units for the publication
*/
template <class X, class FedPtr>
typename std::enable_if_t<helicsType<X>() != helics_type_t::helicsInvalid, std::unique_ptr<Publication>>
make_publication(FedPtr valueFed, const std::string &key, const std::string &units = std::string())
{
    return std::make_unique<Publication> (valueFed, helicsType<X> (), key, units);
}

/** create a pointer to a publication
@tparam X is the type of the publication
@tparam FedPtr a pointer a value Federate
@param locality either LOCAL or GLOBAL defining whether the federate name is prepended or not
@param valueFed pointer to a valid federate
@param key the identifier for the publication
@param units optional units for the publication
*/
template <class X, class FedPtr>
typename std::enable_if_t<helicsType<X> () != helics_type_t::helicsInvalid, std::unique_ptr<Publication>>
make_publication (interface_visibility locality,
                  FedPtr valueFed,
                  const std::string &key,
                  const std::string &units = std::string())
{
    return std::make_unique<Publication> (locality, valueFed, key, helicsType<X> (), units);
}

/** class to handle a publication of an arbitrary type*/
template <class X>
class PublicationT : public PublicationBase
{
  public:
    PublicationT () = default;
    /**constructor to build a publication object
    @param[in] valueFed  the ValueFederate to use
    @param[in] key the identifier for the publication
    @param[in] units the units associated with a Federate
    */
    template<class FedPtr>
    PublicationT(FedPtr valueFed, const std::string &key, const std::string &units = std::string())
        : PublicationBase (valueFed, key, typeNameString<X> (), units)
    {
    }
    /**constructor to build a publication object
    @param[in] valueFed  the ValueFederate to use
    @param[in] key the identifier for the publication
    @param[in] units the units associated with a Federate
    */
    template<class FedPtr>
    PublicationT (interface_visibility locality,
                  FedPtr valueFed,
                  const std::string &key,
                  const std::string &units = std::string())
        : PublicationBase (locality, valueFed, key, typeNameString<X> (), units)
    {
    }
    /** send a value for publication
    @param[in] val the value to publish*/
    virtual void publish (const X &val) const { fed->publish (id, val); }
    /** secondary publish function to allow unit conversion before publication
    @param[in] val the value to publish
    @param[in] units  the units association with the publication
    */
    virtual void publish (const X &val, const std::string & /*units*/) const
    {
        // TODO:: figure out units
        publish (val);
    }
};

/** class to handle a publication on change for an arbitrary type
but the value is only published in the change is greater than a certain level*/
template <class X>
class PublicationOnChange : public PublicationT<X>
{
  private:
    X publishDelta;  //!< the delta on which to publish a value
    mutable X prev;  //!< the previous value
  public:
    PublicationOnChange () = default;
    /**constructor to build a publishOnChange object
    @param[in] valueFed  the ValueFederate to use
    @param[in] key the identifier for the publication
    @param[in] minChange  the minimum change required to actually publish the value
    @param[in] units the units associated with a Federate
    */
    template <class FedPtr>
    PublicationOnChange(FedPtr valueFed,
        const std::string &key,
        const X &minChange,
                         const std::string &units = std::string())
        : PublicationT<X> (valueFed, key, units), publishDelta (minChange),prev(X())
    {
    }
    /** send a value for publication
    @details the value is only published if it exceeds the specified level
    @param[in] val the value to publish*/
    virtual void publish (const X &val) const override
    {
        if (std::abs (val - prev) >= publishDelta)
        {
            prev = val;
            PublicationT<X>::publish (val);
        }
    }
};
}  // namespace helics
