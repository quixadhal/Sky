(sparkle
  (pattern "[at] <indirect:any-living>")
  (pattern "<string>")
  (pattern "<string> [at] <indirect:any-living>")
  (pattern "[at] <indirect:any-living> <string>")
  (single
    (no-arguments
      (self "You sparkle brightly.")
      (rest "$mcname$ sparkles brightly.")
    )
  )
  (targeted
      (arguments
      (arguments brightly,lovely,happily,mischievously,cheekily,idiotically,wryly,sceptically,
                 derisively,annoyingly,absentmindedly,whilst you work,distractedly,pointlessly,
                 silently,loudly,rudely,with laughter,vaguely,obviously)
      (self   "You sparkle $arg$ at $hcname$.")
      (target "$mcname$ sparkles $arg$ at you.")
      (rest  "$mcname$ sparkles $arg$ at $hcname$.")
    )
  )
)

