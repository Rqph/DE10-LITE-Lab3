library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity lab3 is
	port(
		clk : in std_logic := '0';
		reset_n : in std_logic :='0';
		i2c_scl : inout std_logic :='X';
		i2c_sda : inout std_logic :='X';
		i2c_cs_n : out std_logic := '1';
		seg1 : out std_logic_vector(6 downto 0);
		seg2 : out std_logic_vector(6 downto 0);
		seg3 : out std_logic_vector(6 downto 0);
		seg4 : out std_logic_vector(6 downto 0);
		seg5 : out std_logic_vector(6 downto 0);
		seg6 : out std_logic_vector(6 downto 0);
		button : in std_logic
		
		);
end entity lab3;

architecture rit of lab3 is
    component lab3_sys is
        port (
            clk_clk                           : in    std_logic                    := 'X'; -- clk
            i2c_export_0_scl_pad_io           : inout std_logic                    := 'X'; -- scl_pad_io
            i2c_export_0_sda_pad_io           : inout std_logic                    := 'X'; -- sda_pad_io
            reset_reset_n                     : in    std_logic                    := 'X'; -- reset_n
            bouton_external_connection_export : in    std_logic                    := 'X'; -- export
            seg1_external_connection_export   : out   std_logic_vector(3 downto 0);        -- export
            seg3_external_connection_export   : out   std_logic_vector(3 downto 0);        -- export
            seg2_external_connection_export   : out   std_logic_vector(3 downto 0);        -- export
            seg4_external_connection_export   : out   std_logic_vector(3 downto 0);        -- export
            seg5_external_connection_export   : out   std_logic_vector(3 downto 0);        -- export
            seg6_external_connection_export   : out   std_logic_vector(3 downto 0)         -- export
        );
    end component lab3_sys;
	 
	 component bin_7seg is
					port (
						entree : in std_logic_vector(3 downto 0);
						sortie : out std_logic_vector(6 downto 0)
					);
		end component bin_7seg;

	 signal entreebin1 : std_logic_vector(3 downto 0);
	 signal entreebin2 : std_logic_vector(3 downto 0);
	 signal entreebin3 : std_logic_vector(3 downto 0);
	 signal entreebin4 : std_logic_vector(3 downto 0);
	 signal entreebin5 : std_logic_vector(3 downto 0);
	 signal entreebin6 : std_logic_vector(3 downto 0);
	 

begin
    u0 : component lab3_sys
        port map (
            clk_clk                 => clk,              -- clk.clk
            i2c_export_0_scl_pad_io => i2c_scl, 			-- i2c_export_0.scl_pad_io
            i2c_export_0_sda_pad_io => i2c_sda, 			-- i2c_export_0.sda_pad_io
            reset_reset_n           => reset_n,          -- reset.reset_n
				bouton_external_connection_export => button, -- bouton_external_connection.export
            seg1_external_connection_export   => entreebin1,   --   seg1_external_connection.export
            seg3_external_connection_export   => entreebin2,   --   seg3_external_connection.export
            seg2_external_connection_export   => entreebin3,   --   seg2_external_connection.export
            seg4_external_connection_export   => entreebin4,   --   seg4_external_connection.export
            seg5_external_connection_export   => entreebin5,   --   seg5_external_connection.export
            seg6_external_connection_export   => entreebin6    --   seg6_external_connection.export
        );
		  
		u1 : component bin_7seg
			port map(
				entree => entreebin1(3 downto 0),
				sortie => seg1
			);
		
		u2 : component bin_7seg
			port map(
				entree => entreebin2(3 downto 0),
				sortie => seg2
			);
		
		u3 : component bin_7seg
			port map(
				entree => entreebin3(3 downto 0),
				sortie => seg3
			);
		u4 : component bin_7seg
			port map(
				entree => entreebin4(3 downto 0),
				sortie => seg4
			);
		
		u5 : component bin_7seg
			port map(
				entree => entreebin5(3 downto 0),
				sortie => seg5
			);
		u6 : component bin_7seg
			port map(
				entree => entreebin6(3 downto 0),
				sortie => seg6
			);
end architecture;


    

    
