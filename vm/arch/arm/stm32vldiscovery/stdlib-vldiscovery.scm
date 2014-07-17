(define set-led!
  (lambda (value)
    (#%set-led! value)))

(define (clock)   #f)
(define (newline) #f)
(define (display) #f)
(define (readln)  #f)
(define (systick) #f)
(define (sleep x) #f)

(define time->seconds (lambda (t) (quotient t 1000)))
