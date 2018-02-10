/**
 * Paper envelopes that can be written on, closed, sealed and unsealed.  Use
 * a call to "make_envelope()" to set the size and description of the
 * envelope.
 * @author Lemming
 * @started 7/1/2000
 * @change Changed to work with the sealable inherit - Sandoz, 2002.
 */

#define SMALL   3
#define MEDIUM  5
#define LARGE   8

inherit "/std/stationery/sealable";

/**
 * This method queries whether this object is a parcel
 * acceptable by the post office or not.
 * It returns true if it has been sealed.
 * @return 1 if wrapped, 0 if not
 */
int query_is_postage_parcel() { return query_sealed(); }

/**
 * This is the method used to define the envelope and must be called
 * whenever one is created.  The description should be short and
 * should not include the size of the envelope - this will be
 * inferred from the size specified.  A size of 3 or less will be
 * "small", 4 to 5 will be "medium", 6 to 8 is "large" and anything
 * over that will be a small, medium-sized or a large "packet".
 * The default and minimum size is 2.
 * @param description the short description of the envelope
 * @param size the maximum weight of the envelope's contents
 * @param paper the type of paper it has been made of
 * @example
 * // Make "a small pink envelope".
 * make_envelope("pink", 3, "fragrant paper");
 * @example
 * // Make "a large frog flavoured packet"
 * make_envelope("frog flavoured", 10, "tough paper");
 * @see query_envelope_status()
 */
void make_envelope( string description, int size, string paper ) {
    string adj, name;

    if( size < 2 )
        size = 2;

    name = "envelope";

    switch( size ) {
      case 0..SMALL:
        adj = "small";
      break;
      case SMALL + 1..MEDIUM:
        adj = "medium-sized";
      break;
      case MEDIUM + 1..LARGE:
        adj = "large";
      break;
      default:
        name = "packet";
        switch( ( size - LARGE ) / ( LARGE * 2 ) ) {
          case 0 :
            adj = "small";
          break;
          case 1 :
            adj = "medium-sized";
          break;
          default :
            adj = "large";
        }
    }

   set_name( name );
   set_short( adj+" "+description+" "+name );
   add_adjective( adj );
   add_adjective( description );

   set_long("This is a "+adj+" "+description+" "+name+( paper &&
       paper != "" ? ", that has been made out of "+paper : "")+".\n");

   set_weight( 1 + size / ( LARGE * 25 ) );
   set_value( 200 + to_int( 50 * sqrt( to_float(size) ) ) );
   set_max_weight( size );

   set_material("paper");
   add_property("writeable", 1 );

} /* make_envelope() */

/** @ignore */
mixed query_static_auto_load() {
    if( base_name(TO) == __FILE__[0..<3] )
        return int_query_static_auto_load();
    return ([ ]);
} /* query_static_auto_load() */
