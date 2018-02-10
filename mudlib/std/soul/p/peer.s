(peer
  (pattern "[at] <indirect:any-living>")
  (pattern "<string>")
  (pattern "<string> [at] <indirect:any-living>")
  (pattern "[at] <indirect:any-living> <string>")
  (single
    (no-arguments
      (self "You peer around the room.")
      (rest "$mcname$ peers.")
    )
    (arguments
      (arguments  suspiciously, quizzically, anxiously, stupidly, warningly, 
                  expectantly, happily, sadly, desperately, worriedly, 
                  strangely, uncertainly, lopsidedly, lazily, rudely, 
                  disappointedly, fishily, fearfully, like a train, curiously, 
                  tentatively, tiredly, warily, nervously, carefully, owlishly, 
                  dazedly, intently, sceptically, happily, cutely)
      (self "You peer $arg$ around the room.")
      (rest "$mcname$ peers $arg$ around the room.")
    )
  )
  (targeted
    (no-arguments
      (self   "You peer at $hcname$.")
      (target "$mcname$ peers at you.")
      (rest   "$mcname$ peers at $hcname$.")
    )
    (arguments
      (arguments  suspiciously, quizzically, anxiously, stupidly, warningly, 
                  expectantly, happily, sadly, desperately, worriedly, 
                  strangely, uncertainly, lopsidedly, lazily, rudely, 
                  disappointedly, fishily, fearfully, like a train, curiously, 
                  tentatively, tiredly, warily, nervously, carefully, owlishly, 
                  dazedly, intently, sceptically, happily, cutely)
      (self   "You peer $arg$ at $hcname$.")
      (target "$mcname$ peers $arg$ at you.")
      (rest   "$mcname$ peers $arg$ at $hcname$.")
    )
  )
)

