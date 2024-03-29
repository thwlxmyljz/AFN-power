/* Copyright (C) 2003 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef NdbDictionary_H
#define NdbDictionary_H

#include <ndb_types.h>

class Ndb;
struct charset_info_st;
typedef struct charset_info_st CHARSET_INFO;

/**
 * @class NdbDictionary
 * @brief Data dictionary class
 * 
 * The preferred and supported way to create and drop tables and indexes
 * in ndb is through the 
 * MySQL Server (see MySQL reference Manual, section MySQL Cluster).
 *
 * Tables and indexes that are created directly through the 
 * NdbDictionary class
 * can not be viewed from the MySQL Server.
 * Dropping indexes directly via the NdbApi will cause inconsistencies
 * if they were originally created from a MySQL Cluster.
 * 
 * This class supports schema data enquiries such as:
 * -# Enquiries about tables
 *    (Dictionary::getTable, Table::getNoOfColumns, 
 *    Table::getPrimaryKey, and Table::getNoOfPrimaryKeys)
 * -# Enquiries about indexes
 *    (Dictionary::getIndex, Index::getNoOfColumns, 
 *    and Index::getColumn)
 *
 * This class supports schema data definition such as:
 * -# Creating tables (Dictionary::createTable) and table columns
 * -# Dropping tables (Dictionary::dropTable)
 * -# Creating secondary indexes (Dictionary::createIndex)
 * -# Dropping secondary indexes (Dictionary::dropIndex)
 *
 * NdbDictionary has several help (inner) classes to support this:
 * -# NdbDictionary::Dictionary the dictionary handling dictionary objects
 * -# NdbDictionary::Table for creating tables
 * -# NdbDictionary::Column for creating table columns
 * -# NdbDictionary::Index for creating secondary indexes
 *
 * See @ref ndbapi_simple_index.cpp for details of usage.
 */
class NdbDictionary {
public:
  NdbDictionary() {}                          /* Remove gcc warning */
  /**
   * @class Object
   * @brief Meta information about a database object (a table, index, etc)
   */
  class Object {
  public:
    Object() {}                               /* Remove gcc warning */
    virtual ~Object() {}                      /* Remove gcc warning */
    /**
     * Status of object
     */
    enum Status {
      New,                    ///< The object only exist in memory and 
                              ///< has not been created in the NDB Kernel
      Changed,                ///< The object has been modified in memory 
                              ///< and has to be commited in NDB Kernel for 
                              ///< changes to take effect
      Retrieved,              ///< The object exist and has been read 
                              ///< into main memory from NDB Kernel
      Invalid,                ///< The object has been invalidated
                              ///< and should not be used
      Altered                 ///< Table has been altered in NDB kernel
                              ///< but is still valid for usage
    };

    /**
     * Get status of object
     */
    virtual Status getObjectStatus() const = 0;

    /**
     * Get version of object
     */
    virtual int getObjectVersion() const = 0;

    /**
     * Object type
     */
    enum Type {
      TypeUndefined = 0,      ///< Undefined
      SystemTable = 1,        ///< System table
      UserTable = 2,          ///< User table (may be temporary)
      UniqueHashIndex = 3,    ///< Unique un-ordered hash index
      OrderedIndex = 6,       ///< Non-unique ordered index
      HashIndexTrigger = 7,   ///< Index maintenance, internal
      IndexTrigger = 8,       ///< Index maintenance, internal
      SubscriptionTrigger = 9,///< Backup or replication, internal
      ReadOnlyConstraint = 10 ///< Trigger, internal
    };

    /**
     * Object state
     */
    enum State {
      StateUndefined = 0,     ///< Undefined
      StateOffline = 1,       ///< Offline, not usable
      StateBuilding = 2,      ///< Building, not yet usable
      StateDropping = 3,      ///< Offlining or dropping, not usable
      StateOnline = 4,        ///< Online, usable
      StateBackup = 5,        ///< Online, being backuped, usable
      StateBroken = 9         ///< Broken, should be dropped and re-created
    };

    /**
     * Object store
     */
    enum Store {
      StoreUndefined = 0,     ///< Undefined
      StoreTemporary = 1,     ///< Object or data deleted on system restart
      StorePermanent = 2      ///< Permanent. logged to disk
    };

    /**
     * Type of fragmentation.
     *
     * This parameter specifies how data in the table or index will
     * be distributed among the db nodes in the cluster.<br>
     * The bigger the table the more number of fragments should be used.
     * Note that all replicas count as same "fragment".<br>
     * For a table, default is FragAllMedium.  For a unique hash index,
     * default is taken from underlying table and cannot currently
     * be changed.
     */
    enum FragmentType { 
      FragUndefined = 0,      ///< Fragmentation type undefined or default
      FragSingle = 1,         ///< Only one fragment
      FragAllSmall = 2,       ///< One fragment per node, default
      FragAllMedium = 3,      ///< two fragments per node
      FragAllLarge = 4        ///< Four fragments per node.
    };
  };

  class Table; // forward declaration
  
  /**
   * @class Column
   * @brief Represents a column in an NDB Cluster table
   *
   * Each column has a type. The type of a column is determined by a number 
   * of type specifiers.
   * The type specifiers are:
   * - Builtin type
   * - Array length or max length
   * - Precision and scale (not used yet)
   * - Character set for string types
   * - Inline and part sizes for blobs
   *
   * Types in general correspond to MySQL types and their variants.
   * Data formats are same as in MySQL.  NDB API provides no support for
   * constructing such formats.  NDB kernel checks them however.
   */
  class Column {
  public:
    /**
     * The builtin column types
     */
    enum Type {
      Undefined = NDB_TYPE_UNDEFINED,   ///< Undefined 
      Tinyint = NDB_TYPE_TINYINT,       ///< 8 bit. 1 byte signed integer, can be used in array
      Tinyunsigned = NDB_TYPE_TINYUNSIGNED,  ///< 8 bit. 1 byte unsigned integer, can be used in array
      Smallint = NDB_TYPE_SMALLINT,      ///< 16 bit. 2 byte signed integer, can be used in array
      Smallunsigned = NDB_TYPE_SMALLUNSIGNED, ///< 16 bit. 2 byte unsigned integer, can be used in array
      Mediumint = NDB_TYPE_MEDIUMINT,     ///< 24 bit. 3 byte signed integer, can be used in array
      Mediumunsigned = NDB_TYPE_MEDIUMUNSIGNED,///< 24 bit. 3 byte unsigned integer, can be used in array
      Int = NDB_TYPE_INT,           ///< 32 bit. 4 byte signed integer, can be used in array
      Unsigned = NDB_TYPE_UNSIGNED,      ///< 32 bit. 4 byte unsigned integer, can be used in array
      Bigint = NDB_TYPE_BIGINT,        ///< 64 bit. 8 byte signed integer, can be used in array
      Bigunsigned = NDB_TYPE_BIGUNSIGNED,   ///< 64 Bit. 8 byte signed integer, can be used in array
      Float = NDB_TYPE_FLOAT,         ///< 32-bit float. 4 bytes float, can be used in array
      Double = NDB_TYPE_DOUBLE,        ///< 64-bit float. 8 byte float, can be used in array
      Olddecimal = NDB_TYPE_OLDDECIMAL,    ///< MySQL < 5.0 signed decimal,  Precision, Scale
      Olddecimalunsigned = NDB_TYPE_OLDDECIMALUNSIGNED,
      Decimal = NDB_TYPE_DECIMAL,    ///< MySQL >= 5.0 signed decimal,  Precision, Scale
      Decimalunsigned = NDB_TYPE_DECIMALUNSIGNED,
      Char = NDB_TYPE_CHAR,          ///< Len. A fixed array of 1-byte chars
      Varchar = NDB_TYPE_VARCHAR,       ///< Length bytes: 1, Max: 255
      Binary = NDB_TYPE_BINARY,        ///< Len
      Varbinary = NDB_TYPE_VARBINARY,     ///< Length bytes: 1, Max: 255
      Datetime = NDB_TYPE_DATETIME,    ///< Precision down to 1 sec (sizeof(Datetime) == 8 bytes )
      Date = NDB_TYPE_DATE,            ///< Precision down to 1 day(sizeof(Date) == 4 bytes )
      Blob = NDB_TYPE_BLOB,        ///< Binary large object (see NdbBlob)
      Text = NDB_TYPE_TEXT,         ///< Text blob
      Bit = NDB_TYPE_BIT,          ///< Bit, length specifies no of bits
      Longvarchar = NDB_TYPE_LONGVARCHAR,  ///< Length bytes: 2, little-endian
      Longvarbinary = NDB_TYPE_LONGVARBINARY, ///< Length bytes: 2, little-endian
      Time = NDB_TYPE_TIME,        ///< Time without date
      Year = NDB_TYPE_YEAR,   ///< Year 1901-2155 (1 byte)
      Timestamp = NDB_TYPE_TIMESTAMP  ///< Unix time
    };

    /** 
     * @name General 
     * @{
     */
    
    /**
     * Get name of column
     * @return  Name of the column
     */
    const char* getName() const;

    /**
     * Get if the column is nullable or not
     */
    bool getNullable() const;
    
    /**
     * Check if column is part of primary key
     */
    bool getPrimaryKey() const;

    /**
     *  Get number of column (horizontal position within table)
     */
    int getColumnNo() const;

    /**
     * Check if column is equal to some other column
     * @param  column  Column to compare with
     * @return  true if column is equal to some other column otherwise false.
     */
    bool equal(const Column& column) const;


    /** @} *******************************************************************/
    /** 
     * @name Get Type Specifiers
     * @{
     */

    /**
     * Get type of column
     */
    Type getType() const;

    /**
     * Get precision of column.
     * @note Only applicable for decimal types
     */
    int getPrecision() const;

    /**
     * Get scale of column.
     * @note Only applicable for decimal types
     */
    int getScale() const;

    /**
     * Get length for column
     * Array length for column or max length for variable length arrays.
     */
    int getLength() const;

    /**
     * For Char or Varchar or Text, get MySQL CHARSET_INFO.  This
     * specifies both character set and collation.  See get_charset()
     * etc in MySQL.  (The cs is not "const" in MySQL).
     */
    CHARSET_INFO* getCharset() const;


    /**
     * For blob, get "inline size" i.e. number of initial bytes
     * to store in table's blob attribute.  This part is normally in
     * main memory and can be indexed and interpreted.
     */
    int getInlineSize() const;

    /**
     * For blob, get "part size" i.e. number of bytes to store in
     * each tuple of the "blob table".  Can be set to zero to omit parts
     * and to allow only inline bytes ("tinyblob").
     */
    int getPartSize() const;

    /**
     * For blob, set or get "stripe size" i.e. number of consecutive
     * <em>parts</em> to store in each node group.
     */
    int getStripeSize() const;

    /**
     * Get size of element
     */
    int getSize() const;

    /** 
     * Check if column is part of partition key
     *
     * A <em>partition key</em> is a set of attributes which are used
     * to distribute the tuples onto the NDB nodes.
     * The partition key uses the NDB Cluster hashing function.
     *
     * An example where this is useful is TPC-C where it might be
     * good to use the warehouse id and district id as the partition key. 
     * This would place all data for a specific district and warehouse 
     * in the same database node.
     *
     * Locally in the fragments the full primary key 
     * will still be used with the hashing algorithm.
     *
     * @return  true then the column is part of 
     *                 the partition key.
     */
    bool getPartitionKey() const;
#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    inline bool getDistributionKey() const { return getPartitionKey(); };
#endif

    /** @} *******************************************************************/


    /** 
     * @name Column creation
     * @{
     *
     * These operations should normally not be performed in an NbdApi program
     * as results will not be visable in the MySQL Server
     * 
     */

    /**
     * Constructor
     * @param   name   Name of column
     */
    Column(const char * name = "");
    /**
     * Copy constructor
     * @param  column  Column to be copied
     */
    Column(const Column& column); 
    ~Column();

    /**
     * Set name of column
     * @param  name  Name of the column
     */
    void setName(const char * name);

    /**
     * Set whether column is nullable or not
     */
    void setNullable(bool);

    /**
     * Set that column is part of primary key
     */
    void setPrimaryKey(bool);

    /**
     * Set type of column
     * @param  type  Type of column
     *
     * @note setType resets <em>all</em> column attributes
     *       to (type dependent) defaults and should be the first
     *       method to call.  Default type is Unsigned.
     */
    void setType(Type type);

    /**
     * Set precision of column.
     * @note Only applicable for decimal types
     */
    void setPrecision(int);

    /**
     * Set scale of column.
     * @note Only applicable for decimal types
     */
    void setScale(int);

    /**
     * Set length for column
     * Array length for column or max length for variable length arrays.
     */
    void setLength(int length);

    /**
     * For Char or Varchar or Text, get MySQL CHARSET_INFO.  This
     * specifies both character set and collation.  See get_charset()
     * etc in MySQL.  (The cs is not "const" in MySQL).
     */
    void setCharset(CHARSET_INFO* cs);

    /**
     * For blob, get "inline size" i.e. number of initial bytes
     * to store in table's blob attribute.  This part is normally in
     * main memory and can be indexed and interpreted.
     */
    void setInlineSize(int size);

    /**
     * For blob, get "part size" i.e. number of bytes to store in
     * each tuple of the "blob table".  Can be set to zero to omit parts
     * and to allow only inline bytes ("tinyblob").
     */
    void setPartSize(int size);

    /**
     * For blob, get "stripe size" i.e. number of consecutive
     * <em>parts</em> to store in each node group.
     */
    void setStripeSize(int size);

    /** 
     * Set partition key
     * @see getPartitionKey
     *
     * @param  enable  If set to true, then the column will be part of 
     *                 the partition key.
     */
    void setPartitionKey(bool enable);
#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    inline void setDistributionKey(bool enable)
    { setPartitionKey(enable); };
#endif

    /** @} *******************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    const Table * getBlobTable() const;

    void setAutoIncrement(bool);
    bool getAutoIncrement() const;
    void setAutoIncrementInitialValue(Uint64 val);
    void setDefaultValue(const char*);   
    const char* getDefaultValue() const;

    static const Column * FRAGMENT;
    static const Column * FRAGMENT_MEMORY;
    static const Column * ROW_COUNT;
    static const Column * COMMIT_COUNT;
    static const Column * ROW_SIZE;
    static const Column * RANGE_NO;
    
    int getSizeInBytes() const;
#endif
    
  private:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    friend class NdbRecAttr;
    friend class NdbColumnImpl;
#endif
    class NdbColumnImpl & m_impl;
    Column(NdbColumnImpl&);
    Column& operator=(const Column&);
  };

#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
  /**
   * ???
   */
  typedef Column Attribute;
#endif
  
  /**
   * @brief Represents a table in NDB Cluster
   *
   * <em>TableSize</em><br>
   * When calculating the data storage one should add the size of all 
   * attributes (each attributeconsumes at least 4 bytes) and also an overhead
   * of 12 byte. Variable size attributes (not supported yet) will have a 
   * size of 12 bytes plus the actual data storage parts where there is an 
   * additional overhead based on the size of the variable part.<br>
   * An example table with 5 attributes: 
   * one 64 bit attribute, one 32 bit attribute, 
   * two 16 bit attributes and one array of 64 8 bits. 
   * This table will consume 
   * 12 (overhead) + 8 + 4 + 2*4 (4 is minimum) + 64 = 96 bytes per record.
   * Additionally an overhead of about 2 % as page headers and waste should 
   * be allocated. Thus, 1 million records should consume 96 MBytes
   * plus the overhead 2 MByte and rounded up to 100 000 kBytes.<br>
   *
   */
  class Table : public Object {
  public:
    /** 
     * @name General
     * @{
     */

    /**
     * Get table name
     */
    const char * getName() const;

    /**
     * Get table id
     */ 
    int getTableId() const;
    
    /**
     * Get column definition via name.
     * @return null if none existing name
     */
    const Column* getColumn(const char * name) const;
    
    /**
     * Get column definition via index in table.
     * @return null if none existing name
     */
    Column* getColumn(const int attributeId);

    /**
     * Get column definition via name.
     * @return null if none existing name
     */
    Column* getColumn(const char * name);
    
    /**
     * Get column definition via index in table.
     * @return null if none existing name
     */
    const Column* getColumn(const int attributeId) const;
    
    /** @} *******************************************************************/
    /**
     * @name Storage
     * @{
     */

    /**
     * If set to false, then the table is a temporary 
     * table and is not logged to disk.
     *
     * In case of a system restart the table will still
     * be defined and exist but will be empty. 
     * Thus no checkpointing and no logging is performed on the table.
     *
     * The default value is true and indicates a normal table 
     * with full checkpointing and logging activated.
     */
    bool getLogging() const;

    /**
     * Get fragmentation type
     */
    FragmentType getFragmentType() const;
    
    /**
     * Get KValue (Hash parameter.)
     * Only allowed value is 6.
     * Later implementations might add flexibility in this parameter.
     */
    int getKValue() const;

    /**
     * Get MinLoadFactor  (Hash parameter.)
     * This value specifies the load factor when starting to shrink 
     * the hash table. 
     * It must be smaller than MaxLoadFactor.
     * Both these factors are given in percentage.
     */
    int getMinLoadFactor() const;

    /**
     * Get MaxLoadFactor  (Hash parameter.)
     * This value specifies the load factor when starting to split 
     * the containers in the local hash tables. 
     * 100 is the maximum which will optimize memory usage.
     * A lower figure will store less information in each container and thus
     * find the key faster but consume more memory.
     */
    int getMaxLoadFactor() const;

    /** @} *******************************************************************/
    /** 
     * @name Other
     * @{
     */
    
    /**
     * Get number of columns in the table
     */
    int getNoOfColumns() const;
    
    /**
     * Get number of primary keys in the table
     */
    int getNoOfPrimaryKeys() const;

    /**
     * Get name of primary key 
     */
    const char* getPrimaryKey(int no) const;
    
    /**
     * Check if table is equal to some other table
     */
    bool equal(const Table&) const;

    /**
     * Get frm file stored with this table
     */
    const void* getFrmData() const;
    Uint32 getFrmLength() const;

    /** @} *******************************************************************/

    /** 
     * @name Table creation
     * @{
     *
     * These methods should normally not be used in an application as
     * the result is not accessible from the MySQL Server
     *
     */

    /**
     * Constructor
     * @param  name   Name of table
     */
    Table(const char * name = "");

    /** 
     * Copy constructor 
     * @param  table  Table to be copied
     */
    Table(const Table& table); 
    virtual ~Table();
    
    /**
     * Assignment operator, deep copy
     * @param  table  Table to be copied
     */
    Table& operator=(const Table& table);

    /**
     * Name of table
     * @param  name  Name of table
     */
    void setName(const char * name);

    /**
     * Add a column definition to a table
     * @note creates a copy
     */
    void addColumn(const Column &);
    
    /**
     * @see NdbDictionary::Table::getLogging.
     */
    void setLogging(bool); 
   
    /**
     * Set fragmentation type
     */
    void setFragmentType(FragmentType);

    /**
     * Set KValue (Hash parameter.)
     * Only allowed value is 6.
     * Later implementations might add flexibility in this parameter.
     */
    void setKValue(int kValue);
    
    /**
     * Set MinLoadFactor  (Hash parameter.)
     * This value specifies the load factor when starting to shrink 
     * the hash table. 
     * It must be smaller than MaxLoadFactor.
     * Both these factors are given in percentage.
     */
    void setMinLoadFactor(int);

    /**
     * Set MaxLoadFactor  (Hash parameter.)
     * This value specifies the load factor when starting to split 
     * the containers in the local hash tables. 
     * 100 is the maximum which will optimize memory usage.
     * A lower figure will store less information in each container and thus
     * find the key faster but consume more memory.
     */
    void setMaxLoadFactor(int);

    /**
     * Get table object type
     */
    Object::Type getObjectType() const;

    /**
     * Get object status
     */
    virtual Object::Status getObjectStatus() const;

    /**
     * Get object version
     */
    virtual int getObjectVersion() const;

    /**
     * Set frm file to store with this table
     */ 
    void setFrm(const void* data, Uint32 len);

    /**
     * Set table object type
     */
    void setObjectType(Object::Type type);

    /** @} *******************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    void setStoredTable(bool x) { setLogging(x); }
    bool getStoredTable() const { return getLogging(); }

    int getRowSizeInBytes() const ;
    int createTableInDb(Ndb*, bool existingEqualIsOk = true) const ;

    int getReplicaCount() const ;
#endif

  private:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    friend class NdbTableImpl;
#endif
    class NdbTableImpl & m_impl;
    Table(NdbTableImpl&);
  };
  
  /**
   * @class Index
   * @brief Represents an index in an NDB Cluster
   */
  class Index : public Object {
  public:
    
    /** 
     * @name Getting Index properties
     * @{
     */

    /**
     * Get the name of an index
     */
    const char * getName() const;
    
    /**
     * Get the name of the table being indexed
     */
    const char * getTable() const;
    
    /**
     * Get the number of columns in the index
     */
    unsigned getNoOfColumns() const;

#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    /**
     * Get the number of columns in the index
     * Depricated, use getNoOfColumns instead.
     */
    int getNoOfIndexColumns() const;
#endif

    /**
     * Get a specific column in the index
     */
    const Column * getColumn(unsigned no) const ;

#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    /**
     * Get a specific column name in the index
     * Depricated, use getColumn instead.
     */
    const char * getIndexColumn(int no) const ;
#endif

    /**
     * Represents type of index
     */
    enum Type {
      Undefined = 0,          ///< Undefined object type (initial value)
      UniqueHashIndex = 3,    ///< Unique un-ordered hash index 
                              ///< (only one currently supported)
      OrderedIndex = 6        ///< Non-unique ordered index
    };

    /**
     * Get index type of the index
     */
    Type getType() const;
    
    /**
     * Check if index is set to be stored on disk
     *
     * @return if true then logging id enabled
     *
     * @note Non-logged indexes are rebuilt at system restart.
     * @note Ordered index does not currently support logging.
     */
    bool getLogging() const;

    /**
     * Get object status
     */
    virtual Object::Status getObjectStatus() const;

    /**
     * Get object version
     */
    virtual int getObjectVersion() const;

    /** @} *******************************************************************/

    /** 
     * @name Index creation
     * @{
     *
     * These methods should normally not be used in an application as
     * the result will not be visible from the MySQL Server
     *
     */

    /**
     *  Constructor
     *  @param  name  Name of index
     */
    Index(const char * name = "");
    virtual ~Index();

    /**
     * Set the name of an index
     */
    void setName(const char * name);

    /**
     * Define the name of the table to be indexed
     */
    void setTable(const char * name);

    /**
     * Add a column to the index definition
     * Note that the order of columns will be in
     * the order they are added (only matters for ordered indexes).
     */
    void addColumn(const Column & c);

    /**
     * Add a column name to the index definition
     * Note that the order of indexes will be in
     * the order they are added (only matters for ordered indexes).
     */
    void addColumnName(const char * name);

#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    /**
     * Add a column name to the index definition
     * Note that the order of indexes will be in
     * the order they are added (only matters for ordered indexes).
     * Depricated, use addColumnName instead.
     */
    void addIndexColumn(const char * name);
#endif

    /**
     * Add several column names to the index definition
     * Note that the order of indexes will be in
     * the order they are added (only matters for ordered indexes).
     */
    void addColumnNames(unsigned noOfNames, const char ** names);

#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    /**
     * Add several column names to the index definition
     * Note that the order of indexes will be in
     * the order they are added (only matters for ordered indexes).
     * Depricated, use addColumnNames instead.
     */
    void addIndexColumns(int noOfNames, const char ** names);
#endif

    /**
     * Set index type of the index
     */
    void setType(Type type);

    /**
     * Enable/Disable index storage on disk
     *
     * @param enable  If enable is set to true, then logging becomes enabled
     *
     * @see NdbDictionary::Index::getLogging
     */
    void setLogging(bool enable); 

#ifndef DOXYGEN_SHOULD_SKIP_DEPRECATED
    void setStoredIndex(bool x) { setLogging(x); }
    bool getStoredIndex() const { return getLogging(); }
#endif
    
    /** @} *******************************************************************/

  private:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    friend class NdbIndexImpl;
#endif
    class NdbIndexImpl & m_impl;
    Index(NdbIndexImpl&);
  };

  /**
   * @brief Represents an Event in NDB Cluster
   *
   */
  class Event : public Object  {
  public:
    /**
     * Specifies the type of database operations an Event listens to
     */
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    /** TableEvent must match 1 << TriggerEvent */
#endif
    enum TableEvent { 
      TE_INSERT=1, ///< Insert event on table
      TE_DELETE=2, ///< Delete event on table
      TE_UPDATE=4, ///< Update event on table
      TE_ALL=7     ///< Any/all event on table (not relevant when 
                   ///< events are received)
    };
    /**
     *  Specifies the durability of an event
     * (future version may supply other types)
     */
    enum EventDurability { 
      ED_UNDEFINED
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
      = 0
#endif
#if 0 // not supported
      ,ED_SESSION = 1, 
      // Only this API can use it
      // and it's deleted after api has disconnected or ndb has restarted
      
      ED_TEMPORARY = 2
      // All API's can use it,
      // But's its removed when ndb is restarted
#endif
      ,ED_PERMANENT    ///< All API's can use it.
                       ///< It's still defined after a cluster system restart
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
      = 3
#endif
    };

    /**
     *  Constructor
     *  @param  name  Name of event
     */
    Event(const char *name);
    /**
     *  Constructor
     *  @param  name  Name of event
     *  @param  table Reference retrieved from NdbDictionary
     */
    Event(const char *name, const NdbDictionary::Table& table);
    virtual ~Event();
    /**
     * Set unique identifier for the event
     */
    void setName(const char *name);
    /**
     * Get unique identifier for the event
     */
    const char *getName() const;
    /**
     * Define table on which events should be detected
     *
     * @note calling this method will default to detection
     *       of events on all columns. Calling subsequent
     *       addEventColumn calls will override this.
     *
     * @param table reference retrieved from NdbDictionary
     */
    void setTable(const NdbDictionary::Table& table);
    /**
     * Set table for which events should be detected
     *
     * @note preferred way is using setTable(const NdbDictionary::Table&)
     *       or constructor with table object parameter
     */
    void setTable(const char *tableName);
    /**
     * Get table name for events
     *
     * @return table name
     */
    const char* getTableName() const;
    /**
     * Add type of event that should be detected
     */
    void addTableEvent(const TableEvent te);
    /**
     * Set durability of the event
     */
    void setDurability(EventDurability);
    /**
     * Get durability of the event
     */
    EventDurability getDurability() const;
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    void addColumn(const Column &c);
#endif
    /**
     * Add a column on which events should be detected
     *
     * @param attrId Column id
     *
     * @note errors will mot be detected until createEvent() is called
     */
    void addEventColumn(unsigned attrId);
    /**
     * Add a column on which events should be detected
     *
     * @param columnName Column name
     *
     * @note errors will not be detected until createEvent() is called
     */
    void addEventColumn(const char * columnName);
    /**
     * Add several columns on which events should be detected
     *
     * @param n Number of columns
     * @param columnNames Column names
     *
     * @note errors will mot be detected until 
     *       NdbDictionary::Dictionary::createEvent() is called
     */
    void addEventColumns(int n, const char ** columnNames);

    /**
     * Get no of columns defined in an Event
     *
     * @return Number of columns, -1 on error
     */
    int getNoOfEventColumns() const;

    /**
     * Get object status
     */
    virtual Object::Status getObjectStatus() const;

    /**
     * Get object version
     */
    virtual int getObjectVersion() const;

#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    void print();
#endif

  private:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    friend class NdbEventImpl;
    friend class NdbEventOperationImpl;
#endif
    class NdbEventImpl & m_impl;
    Event(NdbEventImpl&);
  };

  /**
   * @class Dictionary
   * @brief Dictionary for defining and retreiving meta data
   */
  class Dictionary {
  public:
    /**
     * @class List
     * @brief Structure for retrieving lists of object names
     */
    struct List {
      /**
       * @struct  Element
       * @brief   Object to be stored in an NdbDictionary::Dictionary::List
       */
      struct Element {
	unsigned id;            ///< Id of object
        Object::Type type;      ///< Type of object
        Object::State state;    ///< State of object
        Object::Store store;    ///< How object is stored
	char * database;        ///< In what database the object resides 
	char * schema;          ///< What schema the object is defined in
	char * name;            ///< Name of object
        Element() :
          id(0),
          type(Object::TypeUndefined),
          state(Object::StateUndefined),
          store(Object::StoreUndefined),
	  database(0),
	  schema(0),
          name(0) {
        }
      };
      unsigned count;           ///< Number of elements in list
      Element * elements;       ///< Pointer to array of elements
      List() : count(0), elements(0) {}
      ~List() {
        if (elements != 0) {
          for (unsigned i = 0; i < count; i++) {
            delete[] elements[i].database;
            delete[] elements[i].schema;
            delete[] elements[i].name;
            elements[i].name = 0;
          }
          delete[] elements;
          count = 0;
          elements = 0;
        }
      }
    };

    /** 
     * @name General
     * @{
     */

    /**
     * Fetch list of all objects, optionally restricted to given type.
     *
     * @param list   List of objects returned in the dictionary
     * @param type   Restrict returned list to only contain objects of
     *               this type
     *
     * @return       -1 if error.
     *
     */
    int listObjects(List & list, Object::Type type = Object::TypeUndefined);
    int listObjects(List & list,
		    Object::Type type = Object::TypeUndefined) const;

    /**
     * Get the latest error
     *
     * @return   Error object.
     */			     
    const struct NdbError & getNdbError() const;

    /** @} *******************************************************************/

    /** 
     * @name Retrieving references to Tables and Indexes
     * @{
     */

    /**
     * Get table with given name, NULL if undefined
     * @param name   Name of table to get
     * @return table if successful otherwise NULL.
     */
    const Table * getTable(const char * name) const;

    /**
     * Get index with given name, NULL if undefined
     * @param indexName  Name of index to get.
     * @param tableName  Name of table that index belongs to.
     * @return  index if successful, otherwise 0.
     */
    const Index * getIndex(const char * indexName,
			   const char * tableName) const;

    /**
     * Fetch list of indexes of given table.
     * @param list  Reference to list where to store the listed indexes
     * @param tableName  Name of table that index belongs to.
     * @return  0 if successful, otherwise -1
     */
    int listIndexes(List & list, const char * tableName);
    int listIndexes(List & list, const char * tableName) const;

    /** @} *******************************************************************/
    /** 
     * @name Events
     * @{
     */
    
    /**
     * Create event given defined Event instance
     * @param event Event to create
     * @return 0 if successful otherwise -1.
     */
    int createEvent(const Event &event);

    /**
     * Drop event with given name
     * @param eventName  Name of event to drop.
     * @return 0 if successful otherwise -1.
     */
    int dropEvent(const char * eventName);
    
    /**
     * Get event with given name.
     * @param eventName  Name of event to get.
     * @return an Event if successful, otherwise NULL.
     */
    const Event * getEvent(const char * eventName);

    /** @} *******************************************************************/

    /** 
     * @name Table creation
     * @{
     *
     * These methods should normally not be used in an application as
     * the result will not be visible from the MySQL Server
     */

    /**
     * Create defined table given defined Table instance
     * @param table Table to create
     * @return 0 if successful otherwise -1.
     */
    int createTable(const Table &table);

    /**
     * Drop table given retrieved Table instance
     * @param table Table to drop
     * @return 0 if successful otherwise -1.
     */
    int dropTable(Table & table);

    /**
     * Drop table given table name
     * @param name   Name of table to drop 
     * @return 0 if successful otherwise -1.
     */
    int dropTable(const char * name);
    
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    /**
     * Alter defined table given defined Table instance
     * @param table Table to alter
     * @return  -2 (incompatible version) <br>
     *          -1 general error          <br>
     *           0 success                 
     */
    int alterTable(const Table &table);

    /**
     * Invalidate cached table object
     * @param name  Name of table to invalidate
     */
    void invalidateTable(const char * name);
#endif

    /**
     * Remove table from local cache
     */
    void removeCachedTable(const char * table);
    /**
     * Remove index from local cache
     */
    void removeCachedIndex(const char * index, const char * table);

    
    /** @} *******************************************************************/
    /** 
     * @name Index creation
     * @{
     *
     * These methods should normally not be used in an application as
     * the result will not be visible from the MySQL Server
     *
     */
    
    /**
     * Create index given defined Index instance
     * @param index Index to create
     * @return 0 if successful otherwise -1.
     */
    int createIndex(const Index &index);

    /**
     * Drop index with given name
     * @param indexName  Name of index to drop.
     * @param tableName  Name of table that index belongs to.
     * @return 0 if successful otherwise -1.
     */
    int dropIndex(const char * indexName,
		  const char * tableName);
    
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    /**
     * Invalidate cached index object
     */
    void invalidateIndex(const char * indexName,
                         const char * tableName);
#endif

    /** @} *******************************************************************/

  protected:
    Dictionary(Ndb & ndb);
    ~Dictionary();
    
  private:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    friend class NdbDictionaryImpl;
    friend class UtilTransactions;
    friend class NdbBlob;
#endif
    class NdbDictionaryImpl & m_impl;
    Dictionary(NdbDictionaryImpl&);
    const Table * getIndexTable(const char * indexName, 
				const char * tableName) const;
  public:
#ifndef DOXYGEN_SHOULD_SKIP_INTERNAL
    const Table * getTable(const char * name, void **data) const;
    void set_local_table_data_size(unsigned sz);
#endif
  };
};

class NdbOut& operator <<(class NdbOut& out, const NdbDictionary::Column& col);

#endif
