(define loop
  (lambda (val)
    (#%sleep 100)
    (#%set-led! (remainder val 2))
    (loop (+ 1 val))))

(#%sleep 1000)
(#%set-led! 1)
(#%sleep 1000)
(#%set-led! 0)

(loop 1)
