;;(sleep 1000)
(set-led! 0 0)
(sleep 1000)
(set-led! 0 1)

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

(loop 0)
