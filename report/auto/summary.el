(TeX-add-style-hook
 "summary"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("revtex4-1" "tightenline" "notitlepage" "nofootinbib")))
   (TeX-run-style-hooks
    "latex2e"
    "revtex4-1"
    "revtex4-110"
    "graphicx"
    "amsmath"
    "subcaption"
    "sidecap"
    "empheq")
   (TeX-add-symbols
    '("comm" 1)
    '("bra" 1)
    '("ket" 1)
    '("qav" 1)
    "spinup"
    "spindown"
    "KSAE")
   (LaTeX-add-bibliographies
    "bibliography"))
 :latex)

