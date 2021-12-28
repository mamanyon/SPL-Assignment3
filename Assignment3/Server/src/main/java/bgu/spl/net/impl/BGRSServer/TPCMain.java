package bgu.spl.net.impl.BGRSServer;

import bgu.spl.net.impl.BGRS.MessageEncoderDecoderImpl;
import bgu.spl.net.impl.BGRS.MessagingProtocolImpl;
import bgu.spl.net.srv.Server;

public class TPCMain {
    public static void main(String[] args) {
        int port = Integer.parseInt(args[0]);
        Server.threadPerClient(
                port,
                () -> new MessagingProtocolImpl<>(),
                ()-> new MessageEncoderDecoderImpl()
        ).serve();
    }
}
