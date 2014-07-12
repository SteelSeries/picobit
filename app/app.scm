(define loop
  (lambda (val)
    (#%sleep 100)
    (set-led! 3 (remainder val 2))
    (loop (+ 1 val))))

(#%sleep 1000)
(set-led! 0 0)
(#%sleep 1000)
(set-led! 0 1)

(display "Test\r\n")

;;(loop 1)
