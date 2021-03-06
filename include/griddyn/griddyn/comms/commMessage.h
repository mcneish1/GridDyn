/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace griddyn
{
class CommPayload
{
  public:
    virtual ~CommPayload () = default;

    template <class Archive>
    void serialize (Archive & /*ar*/) {} // stateless class serialized due to derived classes

    virtual std::string to_string (uint32_t type, uint32_t code) const = 0;
    virtual void from_string (uint32_t type, uint32_t code, const std::string &fromString, size_t offset = 0) = 0;
};

#define BASE_RELAY_MESSAGE_NUMBER 400

/** basic message class */
class commMessage
{
  public:
    /** define the most basic of message types*/
    enum comm_message_type_t : std::uint32_t
    {
        ignoreMessageType = 0,  //!< a message that can be ignored
        pingMessageType = 1,  //!< a message sending a ping
        replyMessageType = 2,  //!< a message responding to a ping
        unknownMessageType = 0xFFFFFFFF,  //!< I don't know what this message means
    };
    enum relay_message_type_t : std::uint32_t
    {
        NO_EVENT = BASE_RELAY_MESSAGE_NUMBER,
        LOCAL_FAULT_EVENT = BASE_RELAY_MESSAGE_NUMBER + 3,
        REMOTE_FAULT_EVENT = BASE_RELAY_MESSAGE_NUMBER + 4,
        BREAKER_TRIP_EVENT = BASE_RELAY_MESSAGE_NUMBER + 5,
        BREAKER_CLOSE_EVENT = BASE_RELAY_MESSAGE_NUMBER + 6,
        LOCAL_FAULT_CLEARED = BASE_RELAY_MESSAGE_NUMBER + 7,
        REMOTE_FAULT_CLEARED = BASE_RELAY_MESSAGE_NUMBER + 8,
        BREAKER_TRIP_COMMAND = BASE_RELAY_MESSAGE_NUMBER + 9,
        BREAKER_CLOSE_COMMAND = BASE_RELAY_MESSAGE_NUMBER + 10,
        BREAKER_OOS_COMMAND = BASE_RELAY_MESSAGE_NUMBER + 11,
        ALARM_TRIGGER_EVENT = BASE_RELAY_MESSAGE_NUMBER + 12,
        ALARM_CLEARED_EVENT = BASE_RELAY_MESSAGE_NUMBER + 13,
    };

    /**constructor from a message type */
    commMessage () = default;
    explicit commMessage (std::uint32_t type);
    commMessage (std::uint32_t type, std::uint32_t code);
    /** get the message type*/
    std::uint32_t getMessageType () const { return m_messageType; }
    /** explicitly set the message type of a message object*/
    void setMessageType (std::uint32_t type) { m_messageType = type; }

    template <class payLoadType>
    payLoadType *getPayload ()
    {
        switch (ptype)
        {
        case payloadType_t::none:
        default:
            return nullptr;
        case payloadType_t::shared:
            return (payload) ? dynamic_cast<payLoadType *> (payload.get ()) : nullptr;
        case payloadType_t::vector:
            return reinterpret_cast<payLoadType *> (payload_V.data ());
        case payloadType_t::raw:
            return reinterpret_cast<payLoadType *> (payloadData);
        }
    }

    auto getPayloadPointer () { return payload; }

    void setPayload (std::shared_ptr<CommPayload> ptr)
    {
        ptype = (ptr) ? payloadType_t::shared : payloadType_t::none;
        payload = std::move (ptr);
    }
    void setPayload (const std::vector<char> &vec)
    {
        payload_V = vec;
        ptype = payloadType_t::vector;
    }
    void setPayload (std::vector<char> &&vec)
    {
        payload_V = std::move (vec);
        ptype = payloadType_t::vector;
    }
    void setPayload (void *data, size_t dsize)
    {
        payloadData = data;
        payloadSize = dsize;
        if ((data == nullptr) || (dsize == 0))
        {
            ptype = payloadType_t::none;
        }
        else
        {
            ptype = payloadType_t::raw;
        }
    }
    /** generate a string describing the message*/
    std::string to_string () const;
    /** load a message definition from a string*/
    void from_string (const std::string &fromString);

  private:
    std::uint32_t m_messageType = ignoreMessageType;  //!< the actual type of the message
  public:
    std::uint32_t code = 0xFFFFFFFF;

  private:
    enum class payloadType_t : uint8_t
    {
        none = 0,
        shared = 1,
        vector = 2,
        raw = 3
    };
    payloadType_t ptype = payloadType_t::none;
    std::shared_ptr<CommPayload> payload;
    void *payloadData = nullptr;  //!< blob pointer for payload data
    size_t payloadSize = 0;  //!< blob size;
    std::vector<char> payload_V;  // payload as a vector
};

// defining a number of alarm codes
enum alarmCode
{
    OVERCURRENT_ALARM = 101,
    UNDERCURRENT_ALARM = 102,
    OVERVOLTAGE_ALARM = 111,
    UNDERVOLTAGE_ALARM = 112,
    TEMPERATURE_ALARM1 = 201,
    TEMPERATURE_ALARM2 = 202,
    UNDERFREQUENCY_ALARM = 301,
    OVERFREQUENCY_ALARM = 303,

};

std::uint32_t getAlarmCode (const std::string &alarmStr);

// component factory is a template class that inherits from cFactory to actually to the construction of a specific
// object

class MessageTypeRegistry
{
  public:
    /** destructor*/
    ~MessageTypeRegistry () = default;
    /** get a pointer to the underlying factory*/
    static MessageTypeRegistry &instance ();
    /** insert a factory in the coreMessageFactory
    @param[in] name the string used to find the message factory in subsequent operations
    @param[in] mf pointer to a message factory to store in the core factory*/
    void registerType (const std::string &name, std::uint32_t);

    uint32_t getType (const std::string &name) const;
    std::string getTypeString (int32_t type) const;

  private:
    /** private constructor defined in a singleton class*/
    MessageTypeRegistry () = default;

    std::unordered_map<std::string, uint32_t> typeMapA;  //!< the map of the strings and types
    std::unordered_map<uint32_t, std::string> typeMapB;  //!< the map of the strings and types
};

class typeRegister
{
  private:
    std::uint32_t type_;

  public:
    typeRegister (const std::string &name, std::uint32_t type) : type_ (type)
    {
        MessageTypeRegistry::instance ().registerType (name, type);
    };
    // implicit conversion
    operator uint32_t () { return type_; }
};

/** macro used to register types with the type registry*/
#define REGISTER_MESSAGE_TYPE(name, namestr, type) static const typeRegister name (namestr, type)

/** class definitions for the message factories that can create the message
messageFactory is a virtual base class for message Construction functions
*/
class payloadFactory
{
  public:
    std::string name;  //!< the name of the factory
    /** constructor taking the name as an argument*/
    explicit payloadFactory (const std::string &typeName) : name (typeName) {}
    /** virtual destructor*/
    virtual ~payloadFactory () = default;

    /** build a default message*/
    virtual std::shared_ptr<CommPayload> makePayload () = 0;
    /** check if a given message type is within the valid range of a specific message object*/
    virtual bool inRange (std::uint32_t) const { return true; }

    virtual std::uint32_t range () const
    {
        return 0xFFFFFFF0;
    }  // return a very big range but leave a little room for special message codes
};

// TODO:: merge with the coreTypeFactory and other templates May not be able to with the extra functions required
// create a high level object factory for the coreMessageFactory class
typedef std::unordered_map<std::string, payloadFactory *> mfMap;
/** core message factory class for building messages of a specified type
 */
class corePayloadFactory
{
  public:
    /** destructor*/
    ~corePayloadFactory () = default;
    /** get a pointer to the underlying factory*/
    static corePayloadFactory &instance ();
    /** insert a factory in the coreMessageFactory
    @param[in] name the string used to find the message factory in subsequent operations
    @param[in] mf pointer to a message factory to store in the core factory*/
    void registerFactory (std::string name, payloadFactory *mf);
    /** insert a factory in the coreMessageFactory
    @param[in] mf pointer to a message factory to store in the core factory*/
    void registerFactory (payloadFactory *mf);
    /** get a list of all the valid message types*/
    std::vector<std::string> getPayloadTypeNames ();
    /** build a default message of the type defined in messageType*/
    std::shared_ptr<CommPayload> createPayload (const std::string &messageType);
    /** build a default message of the type defined in messageType
    @param messageType string describing the class of messages
    @param type the specific message code for the message*/
    std::shared_ptr<CommPayload> createPayload (const std::string &messageType, std::uint32_t type);
    /** build a message payload of the specific type, deriving the general type from the valid ranges of specific
    types defining in the factory
  @param type the specific message code for the message*/
    std::shared_ptr<CommPayload> createPayload (std::uint32_t type);
    /** get a pointer to a specific factory*/
    payloadFactory *getFactory (const std::string &factoryName);
    /** get a pointer to a factory that builds a specific type of message*/
    payloadFactory *getFactory (std::uint32_t type);
    /** check if a string represents a valid message class*/
    bool isValidMessage (const std::string &messageType);

  private:
    /** private constructor defined in a singleton class*/
    corePayloadFactory () = default;

    mfMap m_factoryMap;  //!< the map containing the factories from a string
};

/** template for making a specific message from the factory
 */
template <class PayloadType, std::uint32_t minCodeValue, std::uint32_t maxCodeValue>
class dPayloadFactory : public payloadFactory
{
    static_assert (std::is_base_of<CommPayload, PayloadType>::value,
                   "factory class must have commMessage as base");

  public:
    explicit dPayloadFactory (const std::string &typeName) : payloadFactory (typeName)
    {
        corePayloadFactory::instance ().registerFactory (typeName, this);
    }

    virtual std::shared_ptr<CommPayload> makePayload () override
    {
        std::shared_ptr<CommPayload> cm = std::make_shared<PayloadType> ();
        return cm;
    }

    /** default generator for a specific instantiation of a message factory*/
    std::shared_ptr<PayloadType> makeTypePayload () { return (std::make_shared<PayloadType> ()); }

    virtual bool inRange (std::uint32_t code) const override
    {
        return ((code >= minCodeValue) && (code <= maxCodeValue));
    }
    virtual std::uint32_t range () const override { return (maxCodeValue - minCodeValue); }
};

}  // namespace griddyn
