#pragma once

#include "postres_types.h"

typedef struct Attribute {
    /* OID of relation containing this attribute */
    Oid			attrelid;

    /* name of attribute */
    NameData	attname;

    /*
	 * atttypid is the OID of the instance in Catalog Class pg_type that
	 * defines the data type of this attribute (e.g. int4).  Information in
	 * that instance is redundant with the attlen, attbyval, and attalign
	 * attributes of this instance, so they had better match or Postgres will
	 * fail.  In an entry for a dropped column, this field is set to zero
	 * since the pg_type entry may no longer exist; but we rely on attlen,
	 * attbyval, and attalign to still tell us how large the values in the
	 * table are.
	 */
    Oid			atttypid;

    /*
	 * attlen is a copy of the typlen field from pg_type for this attribute.
	 * See atttypid comments above.
	 */
    int16		attlen;

    /*
	 * attnum is the "attribute number" for the attribute:	A value that
	 * uniquely identifies this attribute within its class. For user
	 * attributes, Attribute numbers are greater than 0 and not greater than
	 * the number of attributes in the class. I.e. if the Class pg_class says
	 * that Class XYZ has 10 attributes, then the user attribute numbers in
	 * Class pg_attribute must be 1-10.
	 *
	 * System attributes have attribute numbers less than 0 that are unique
	 * within the class, but not constrained to any particular range.
	 *
	 * Note that (attnum - 1) is often used as the index to an array.
	 */
	int16		attnum;

    /*
	 * atttypmod records type-specific data supplied at table creation time
	 * (for example, the max length of a varchar field).  It is passed to
	 * type-specific input and output functions as the third argument. The
	 * value will generally be -1 for types that do not need typmod.
	 */
	int32		atttypmod;

    /*
	 * attndims is the declared number of dimensions, if an array type,
	 * otherwise zero.
	 */
	int16		attndims;

    /*
	 * attbyval is a copy of the typbyval field from pg_type for this
	 * attribute.  See atttypid comments above.
	 */
	bool		attbyval;

    /*
	 * attalign is a copy of the typalign field from pg_type for this
	 * attribute.  See atttypid comments above.
	 */
	char		attalign;

    /*----------
	 * attstorage tells for VARLENA attributes, what the heap access
	 * methods can do to it if a given tuple doesn't fit into a page.
	 * Possible values are as for pg_type.typstorage (see TYPSTORAGE macros).
	 *----------
	 */
	char		attstorage;

    /*
	 * attcompression sets the current compression method of the attribute.
	 * Typically this is InvalidCompressionMethod ('\0') to specify use of the
	 * current default setting (see default_toast_compression).  Otherwise,
	 * 'p' selects pglz compression, while 'l' selects LZ4 compression.
	 * However, this field is ignored whenever attstorage does not allow
	 * compression.
	 */
	char		attcompression;

    /* This flag represents the "NOT NULL" constraint */
	bool		attnotnull;

    /* Has DEFAULT value or not */
	bool		atthasdef;

    /* Has a missing value or not */
	bool		atthasmissing;

    /* One of the ATTRIBUTE_IDENTITY_* constants below, or '\0' */
	char		attidentity;

    /* One of the ATTRIBUTE_GENERATED_* constants below, or '\0' */
	char		attgenerated;

    /* Is dropped (ie, logically invisible) or not */
	bool		attisdropped;

    /*
	 * This flag specifies whether this column has ever had a local
	 * definition.  It is set for normal non-inherited columns, but also for
	 * columns that are inherited from parents if also explicitly listed in
	 * CREATE TABLE INHERITS.  It is also set when inheritance is removed from
	 * a table with ALTER TABLE NO INHERIT.  If the flag is set, the column is
	 * not dropped by a parent's DROP COLUMN even if this causes the column's
	 * attinhcount to become zero.
	 */
	bool		attislocal;

    /* Number of times inherited from direct parent relation(s) */
	int16		attinhcount;

    /* attribute's collation, if any */
	Oid			attcollation;
} Attribute;