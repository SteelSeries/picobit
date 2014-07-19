(define read-loop
  (lambda ()
    (map (lambda (x) (displayln x))
	 (list "Readline test:" (readln)))
    (read-loop)))
