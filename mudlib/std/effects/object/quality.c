/**
 * This effect is used to impart the attribute of "quality" upon an
 * object, be it weapon, armour or normal object.  There are a couple
 * main effects of quality upon an object:
 * <ul>
 * <li>
 * <b>Sturdiness</b>. A higher quality object will suffer less damage
 * than it would normally. Lower quality suffers more damage.
 * <li>
 * <b>Value</b>. A higher quality object is worth more. Lower quality
 * brings a lower price.
 * <li>
 * <b>Damage Inflicted</b>. A higher quality weapon has better balance,
 * holds a better edge (for sharp weapons), has a better designed
 * striking surface (for blunt weapons), and better grips, and thus
 * will do more damage than a normal weapon.  A low quality weapon,
 * on the other hand, might have a loose handle, poor edge or even
 * a warped blade.
 * <li>
 * <b>Enchantment potential</b>. A higher quality weapon, due to its
 * superior form and structure, will hold a higher enchantment than
 * something put together by some drunken twonk from Slice.
 * <li>
 * <b>Appearance</b>. The quality of the item is obvious to those who
 * inspect it.
 * </ul>
 * All these modifications to normal behavior are on a logarithmic scale,
 * thus the range of possible quality numbers ranges from +infinity to
 * -infinity, but in actual practice only +10 to -10 are meaningful.
 * Even so, the further you get from quality 0 (normal in all respects)
 * the more rare it should be.  Most shopkeepers will not carry poor
 * quality merchandice unless they are in dire straits, and there are
 * extremely few people on the disc capable of even approaching the upper
 * quality ratings.  Even then, they only do so by the grace of the Gods.
 * @example wossname->add_effect(EFFECT_PATH+"object/quality.c",4)
 * @classification object.quality
 * @author Daxa (Greg Craff)
 * @started Sun 2200 22 Mar 1998
 */

#include <effect.h>
#include "path.h"

/**
 * @ignore
 */
string query_shadow_ob() { return SHADOWS + "quality"; }

/**
 * @ignore
 */
string query_classification() { return "object.quality"; }

/**
 * @ignore
 */
int query_indefinite() { return 1; }

/**
 * This function is called by add_effect.
 * @param wossname The object receiving the effect; don't worry about it.
 * @param quality The integer indicating quality.  This is the only thing
 * you worry about.
 * @param id The effect id on the object receiving the effect; ignore it.
 */
int beginning( object wossname, int quality, int id)
{
  wossname->add_extra_look( this_object() );
}

void end( object wossname, int quality, int id)
{
  wossname->remove_extra_look( this_object() );
}

int merge_effect( object wossname, int old_quality, int new_quality, int id)
{
  // Necessary to handle quality = 0
  if (new_quality == 0)
  {
//    function bing = (: $(wossname), "set_arg_of", $(id), 0 :);
    function bing = (: $(wossname)->set_arg_of($(id), 0) :);
    call_out(bing, 0);
    return 1;
  }
  else
  return new_quality;
}

int restart( object wossname, int quality, int id)
{
  wossname->add_extra_look( this_object() );
}

/**
 * @ignore
 */
string extra_look( object wossname )
{
  return capitalize(wossname->query_pronoun()) + 
    " appears to be of "+ wossname->quality() + " quality.\n";
}
