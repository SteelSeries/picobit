(define loop
  (lambda (val)
    (#%sleep 1000)
    (set-led! 3 (remainder val 2))
    (loop (+ 1 val))))

(#%sleep 1000)
(set-led! 0 0)
(#%sleep 1000)
(set-led! 0 1)

(display "Test\r\n")
(display '(1 2 3 4 5))
(newline)
(pp '(list (+ 1 2) (/ 1 2)))
(display "loop")
(newline)

(loop 1)
