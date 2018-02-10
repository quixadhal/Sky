(swoon
  (pattern "[at] <indirect:any-living>")
  (pattern "<string>")
  (pattern "<string> [at] <indirect:any-living>")
  (pattern "[at] <indirect:any-living> <string>")
  (single
    (no-arguments
      (self "You swoon.")
      (rest "$mcname$ swoons.")
    )
    (arguments
       (arguments like a swallow on a warm summers day with the sunlight 
                 streaming through the trees and small fluffy clouds wandering
                 across the deep blue sky,like Ariadne, dramatically, heavily, 
                 desperately, suggestively,melodramatically,like Pinkfish on dried
                 frog pills,happily,backwards,sideways,in some sort of a swoony way)
      (self "You swoon $arg$.")
      (rest "$mcname$ swoons $arg$.")
    )
  )
  (targeted
    (no-arguments
      (self   "You swoon at $hcname$.")
      (target "$mcname$ swoons at you.")
      (rest  "$mcname$ swoons at $hcname$.")
    )
    (arguments
      (arguments like a swallow on a warm summers day with the sunlight
                 streaming through the trees and small fluffy clouds wandering
                 across the deep blue sky,like Ariadne, dramatically, heavily, 
                 desperately, suggestively,melodramatically,like Pinkfish on dried 
                 frog pills,happily,backwards,sideways,in some sort of a swoony way)
      (self   "You swoon $arg$ at $hcname$.")
      (target "$mcname$ swoons $arg$ at you.")
      (rest  "$mcname$ swoons $arg$ at $hcname$.")
    )
  )
)

