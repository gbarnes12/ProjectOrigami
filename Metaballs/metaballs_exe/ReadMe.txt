Metaballs - created 4/21-2001 by Andreas Jönsson at www.AngelCode.com
-------------------------------------------------------------------------------
I've always been fascinated by Metaballs. It's a simple algorithm once you've
learned how it works but it produces such a mesmerizing result. Almost exactly
one year from now I created my first Metaballs, with an average result. A few
weeks ago I had an idea how to improve on Marching Cubes, that would also 
bypass the patent on that algorithm. I needed an application to test my idea on
so I decided to write a new Metaballs demo. 

As it turned out my idea wasn't as good as I hoped. It worked but not better 
than Marching Cubes so I scrapped it. I did however stumble on a way to make 
the Metaballs many times faster than my previous attempt. In this demo I have 
the impressive number of 20 Metaballs moving around the screen at once with 
acceptable framerates. I've never seen any demo with more than 5 Metaballs at 
once. 

As so often before it turns out that the really efficient algorithms are 
equally simple, it's just the way to get to the algorithms that are tough. 
Expect to see an article explaining how I made these Metaballs on my homepage
http://www.AngelCode.com soon.

You'll need to have DirectX 8 installed to run this demo. I've only got one
computer with a GeForce 256 in it so I don't know how it runs on other
machines, but I expect graphic cards that are able to run in windowed mode to 
run this demo resonably well. The framerate might be a little poor on lower end
machines though. With my 500 MHz Celeron I get acceptable framerates so I guess
a slower machine than that would not be recommended.  

The spheremap was photographed by Johan Larsby (http:www.larsby.org/johan) and
portrays part of the funpark in Copenhagen, Denmark, at night. It was modified 
to be better suited for spheremapping. 
 
Please report bugs to me andreas@angelcode.com.

Bug fix - 16/10-2002
-------------------------------------------------------------------------------
Special thanks goes to NoiseCrime, of www.noisecrime.com, who helped me find a 
bug that I wasn't able to reconstruct on my machine. The bug fixed was that of 
missing triangles, visible on for example GeForce2.