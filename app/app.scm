(sleep 1000)
(set-led! 0 0)
(sleep 1000)
(set-led! 0 1)

;; (display "Test\r\n")
;; (display '(1 2 3 4 5))
;; (newline)
;; (pp '(list (+ 1 2) (/ 1 2)))
;; (display "String test. BEGIN.")
;; (newline)

;; (define (foo x)
;;   (+ (string-length x) 1))
;; (display "abc")
;; (display (foo "abc"))
;; (newline)

;; (displayln "-------------------------")

;; (define (f x y) ; one is mutable, the other is not
;;   (set! x 4)
;;   (#%+ x y))
;; (displayln (f 1 6))

;; (displayln "-------------------------")

;; (displayln (map (lambda (x) (+ x 1)) '(1 2 3 4 5)))
;; (displayln (for-each (lambda (x) (set! x (+ x 1))) '(1 2 3 4 5)))

;; (displayln "-------------------------")

;; (define x 255)
;; (define (loop-fixnum x)
;;   (if (> x 0)
;;       (begin (displayln x)
;; 	     (loop-fixnum (- x 1)))))
;; (loop-fixnum x)

;; (displayln "-------------------------")

;; (start-first-process
;;  (lambda ()
;;    (let loop ((a 1))
;;      (begin
;;        (spawn
;;         (lambda ()
;;           (let loop2 ((a2 0))
;;             (yield)
;;             (displayln a2)
;;             (if (= a a2) (exit)
;;                 (loop2 (+ a2 1))))))
;;        (if (= a 10)
;;            (exit)
;;            (loop (+ a 1)))))))

;; (displayln "DONE")

;; (start-first-process
;;  (lambda ()
;;    (let loop ((a 1))
;;      (begin
;;        (spawn
;;         (lambda ()
;;           (let loop2 ((a2 0))
;;             (yield)
;;             (displayln a2)
;;             (if (= a a2) #f
;;                 (loop2 (+ a2 1))))))
;;        (if (= a 10)
;;            (exit)
;;            (loop (+ a 1)))))))

;; (displayln "DONE")

;; (displayln "-------------------------")

(define read-loop
  (lambda ()
    (map (lambda (x) (displayln x))
	 (list "Readline test:" (readln)))
    (read-loop)))

(define (loop val)
  (sleep 50)
  (set-led! 1 (remainder val 2))
  (set-led! 2 (remainder (+ 1 val) 2))
  (let ([rtc (clock)] [clk (systick)])
    (map (lambda (x) (display x))
	 (list val " "
	       rtc " - " clk " = "
	       (- rtc (time->seconds clk)) (newline))))
  (loop (+ 1 val)))

(define lcd-test
  (lambda ()
    (let loop-x ((x 0) (count 0))
      (if (< x 240)
	  (begin
	    (let loop-y ((y 0) (mask 65535))
	      (if (< y 320)
		  (begin
		    (putpixel x y (bitwise-and count mask))
		    (loop-y (+ y 1) (- mask 1)))))
	    (loop-x (+ 1 x) (+ 1 count)))))))

(lcd-test)

;;(read-loop)
;;(loop 0)
